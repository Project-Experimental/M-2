/*
 * Copyright (c) 2021 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include "arch/x86/apic.h"

#include <sys/types.h>
#include <lk/debug.h>
#include <lk/err.h>
#include <lk/reg.h>
#include <lk/trace.h>
#include <lk/init.h>
#include <lib/fixed_point.h>
#include <assert.h>
#include <kernel/thread.h>
#include <platform/interrupts.h>
#include <arch/ops.h>
#include <arch/x86.h>
#include <arch/x86/feature.h>
#include <arch/x86/mp.h>
#include <kernel/spinlock.h>
#include <platform/time.h>
#include <platform/timer.h>
#include <platform/pc/timer.h>
#include <kernel/vm.h>
#include <kernel/mp.h>

#define LOCAL_TRACE 0

#define ENTER_PRINTF() printf("M2::-> Enter %s\n", __PRETTY_FUNCTION__);
#define DEBUG_PRINT(...) printf("M2::-> " __VA_ARGS__);

static kernel::arch::LApic g_lapic;

// TODO: move these callbacks into the shared timer code
static platform_timer_callback t_callback;
static void *callback_arg;

static void lapic_init_percpu(uint level);
static uint32_t lapic_read_current_tick(void);
// static void lapic_timer_init_percpu(uint level);

static enum handler_return lapic_timer_handler(void *arg);
static enum handler_return lapic_spurious_handler(void *arg);
static enum handler_return lapic_generic_handler(void *arg);
static enum handler_return lapic_reschedule_handler(void *arg);

namespace kernel::arch
{

uint32_t LApic::Read(enum lapic_regs reg)
{
    LTRACEF_LEVEL(2, "reg %#x\n", reg);
    if (lapic_x2apic) {
        // TODO: do we need barriers here?
        DEBUG_ASSERT(reg != LAPIC_ICRLO && reg != LAPIC_ICRHI);
        return read_msr(X86_MSR_IA32_X2APIC_BASE + reg / 0x10);
    } else {
        return mmio_read32(lapic_mmio + reg / 4);
    }
}

void LApic::Write(enum lapic_regs reg, uint32_t val)
{
    LTRACEF_LEVEL(2, "reg %#x val %#x\n", reg, val);
    if (lapic_x2apic) {
        DEBUG_ASSERT(reg != LAPIC_ICRLO && reg != LAPIC_ICRHI);
        write_msr(X86_MSR_IA32_X2APIC_BASE + reg / 0x10, val);
    } else {
        mmio_write32(lapic_mmio + reg / 4, val);
    }
}

void LApic::WaitForICRDelivery(void)
{
    LTRACEF_LEVEL(2, "waiting for icr\n");
    uint32_t val;
    do {
        if (lapic_x2apic) {
            val = read_msr(X86_MSR_IA32_X2APIC_BASE + 0x30);
        } else {
            val = Read(LAPIC_ICRLO);
        }
    } while (val & (1u << 12));
}

void LApic::WriteICR(uint32_t low, uint32_t apic_id)
{
    LTRACEF_LEVEL(2, "%#x apic_id %#x\n", low, apic_id);
    if (lapic_x2apic) {
        write_msr(X86_MSR_IA32_X2APIC_BASE + 0x30, ((uint64_t)apic_id << 32) | low);
    } else {
        Write(LAPIC_ICRHI, apic_id << 24);
        Write(LAPIC_ICRLO, low);
        WaitForICRDelivery();
    }
}

uint32_t LApic::ReadCurrentTick(void)
{
    if (!lapic_present) {
        return 0;
    }

    return Read(LAPIC_TCCR);
}

void LApic::InitTimerPerCpu(uint level)
{
    ENTER_PRINTF();
    // check for deadline mode
    if (use_tsc_deadline) {
        // put the timer in TSC deadline and clear the match register
        uint32_t val = (LAPIC_TIMER_MODE_TSC_DEADLINE << 17) | LAPIC_INT_TIMER;

        // DANGER: What the Hell is going on

        Write(LAPIC_TIMER, val);
        write_msr(X86_MSR_IA32_TSC_DEADLINE, 0);
    } else {
        // configure the local timer and make sure it is not set to fire
        uint32_t val = (LAPIC_TIMER_MODE_ONESHOT << 17) | LAPIC_INT_TIMER;
        Write(LAPIC_TIMER, val);
        Write(LAPIC_TICR, 0);
    }

    // register the timer interrupt vector
    register_int_handler_msi(LAPIC_INT_TIMER, &lapic_timer_handler, nullptr, false);
}

void LApic::Init(void)
{
    ENTER_PRINTF();
    lapic_present = x86_feature_test(X86_FEATURE_APIC);
    DEBUG_PRINT("lapic_present: %d\n", lapic_present);
}

void LApic::InitPerCpu(uint level)
{
    ENTER_PRINTF();
    // If we're on a secondary cpu we should have a local apic detected and present
    DEBUG_ASSERT(lapic_present);

    EnableOnLocalCPU();

    // set the spurious vector register
    uint32_t svr = (LAPIC_INT_SPURIOUS | (1u<<8)); // enable
    Write(LAPIC_SVR, svr);

    LTRACEF("lapic svr %#x\n", Read(LAPIC_SVR));

    register_int_handler_msi(LAPIC_INT_SPURIOUS, &lapic_spurious_handler, nullptr, false);
    register_int_handler_msi(LAPIC_INT_GENERIC, &lapic_generic_handler, nullptr, false);
    register_int_handler_msi(LAPIC_INT_RESCHEDULE, &lapic_reschedule_handler, nullptr, false);
}

void LApic::InitPostVM(void)
{
    ENTER_PRINTF();

    if (!lapic_present) {
        return;
    }

    dprintf(INFO, "X86: local apic detected\n");

    // IA32_APIC_BASE_MSR
    uint64_t apic_base = read_msr(X86_MSR_IA32_APIC_BASE);
    LTRACEF("raw apic base msr %#llx\n", apic_base);

    // check for X2APIC feature
    if (x86_feature_test(X86_FEATURE_X2APIC)) {
        lapic_x2apic = true;
        dprintf(INFO, "X86: local apic supports x2APIC mode\n");
    }

    dprintf(INFO, "X86: lapic physical address %#llx\n", apic_base & ~0xfff);

    // make sure the apic is enabled on the first cpu
    EnableOnLocalCPU();

    // map the lapic into the kernel since it's not guaranteed that the physmap covers it
    if (!lapic_x2apic) {
        LTRACEF("mapping lapic into kernel\n");
        status_t err = vmm_alloc_physical(vmm_get_kernel_aspace(), "lapic", PAGE_SIZE, (void **)&lapic_mmio, 0,
                                apic_base & ~0xfff, /* vmm_flags */ 0, ARCH_MMU_FLAG_UNCACHED_DEVICE);
        ASSERT(err == NO_ERROR);
        ASSERT(lapic_mmio != nullptr);
    }

    // semi-hack: re-read the APIC id of the boot cpu make sure the pecpu struct is correct
    // before we go any further, in case the boot cpu's apic id is not 0.
    x86_percpu_t *percpu = x86_get_percpu();
    percpu->apic_id = GetAPICId();
    dprintf(INFO, "X86: boot cpu apic id %u\n", percpu->apic_id);

    // Read the local apic id and version and features
    uint32_t id = Read(LAPIC_ID);
    uint32_t version = Read(LAPIC_VERSION);
    bool eas = version & (1u<<31);
    uint32_t max_lvt = (version >> 16) & 0xff;
    version &= 0xff;
    dprintf(INFO, "X86: local apic id %#x version %#x\n", id, version);
    dprintf(INFO, "X86: local apic max lvt entries %u\n", max_lvt);
    if (eas) {
        dprintf(INFO, "X86: local apic EAS features %#x\n", Read(LAPIC_EXT_FEATURES));
    }

    // Finish up some local initialization that all cpus will want to do
    InitPerCpu(0);
}

status_t LApic::InitTimer(bool tsc_supported)
{
    ENTER_PRINTF();
    if (!lapic_present) {
        return ERR_NOT_FOUND;
    }

    // check for deadline mode
    bool tsc_deadline  = x86_feature_test(X86_FEATURE_TSC_DEADLINE);
    if (tsc_supported && tsc_deadline) {
        dprintf(INFO, "X86: local apic timer supports TSC deadline mode\n");
        use_tsc_deadline = true;
    } else {
        // configure the local timer and make sure it is not set to fire
        uint32_t val = (LAPIC_TIMER_MODE_ONESHOT << 17) | LAPIC_INT_TIMER;
        Write(LAPIC_TIMER, val);

        // calibrate the timer frequency
        Write(LAPIC_TICR, 0xffffffff); // countdown from the max count
        uint32_t lapic_hz = pit_calibrate_lapic(&lapic_read_current_tick);
        Write(LAPIC_TICR, 0);
        printf("X86: local apic timer frequency %uHz\n", lapic_hz);

        fp_32_64_div_32_32(&timebase_to_lapic, lapic_hz, 1000);
        dprintf(INFO, "X86: timebase to local apic timer ratio %u.%08u...\n",
                timebase_to_lapic.l0, timebase_to_lapic.l32);
    }

    InitTimerPerCpu(0);

    return ERR_NOT_FOUND;
}

void LApic::Eoi(uint32_t vector)
{
    ENTER_PRINTF();
    LTRACEF("vector %#x\n", vector);
    if (!lapic_present) {
        return;
    }

    Write(LAPIC_EOI, 0);
}

void LApic::EnableOnLocalCPU(void)
{
    ENTER_PRINTF();
    DEBUG_ASSERT(lapic_present);

    // Make sure the apic is enabled and x2apic mode is set (if supported)
    uint64_t apic_base = read_msr(X86_MSR_IA32_APIC_BASE);
    apic_base |= (1u<<11);
    if (lapic_x2apic) {
        apic_base |= (1u<<10);
    }
    write_msr(X86_MSR_IA32_APIC_BASE, apic_base);
}

uint32_t LApic::GetAPICId(void)
{
    if (!lapic_present) {
        return -1;
    }

    if (lapic_x2apic) {
        return (uint32_t)read_msr(0x802);
    } else {
        return Read(LAPIC_ID) >> 24;
    }
}

void LApic::SetAPICId(uint32_t apic_id)
{
    this->apic_id = apic_id;
}

status_t LApic::SetOneShotTimer(
    platform_timer_callback callback, 
    void *arg, 
    lk_time_t interval
)
{
    ENTER_PRINTF();
    LTRACEF("cpu %u interval %u\n", arch_curr_cpu_num(), interval);

    DEBUG_ASSERT(arch_ints_disabled());

    t_callback = callback;
    callback_arg = arg;

    if (use_tsc_deadline) {
        uint64_t now = __builtin_ia32_rdtsc();
        uint64_t delta = time_to_tsc_ticks(interval);
        uint64_t deadline = now + delta;
        LTRACEF("now %llu delta %llu deadline %llu\n", now, delta, deadline);
        write_msr(X86_MSR_IA32_TSC_DEADLINE, deadline);
    } else {
        // set the initial count, which should trigger the timer
        uint64_t ticks = u64_mul_u32_fp32_64(interval, timebase_to_lapic);
        if (ticks > UINT32_MAX) {
            ticks = UINT32_MAX;
        }

        Write(LAPIC_TICR, ticks & 0xffffffff);
    }

    return NO_ERROR;
}

void LApic::CancelTimer(void)
{
    LTRACE;

    DEBUG_ASSERT(arch_ints_disabled());

    if (use_tsc_deadline) {
        write_msr(X86_MSR_IA32_TSC_DEADLINE, 0);
    } else {
        Write(LAPIC_TICR, 0);
    }
}

void LApic::SendInitIpi(bool level)
{
    if (!lapic_present) {
        return;
    }

    // Level triggered mode, level according to arg, INIT delivery mode, no shorthand
    WriteICR((1u << 15) | (level ? (1u << 14) : 0) | (5u << 8), apic_id);
}

void LApic::SendStartupIpi(uint32_t startup_vec)
{
    if (!lapic_present) {
        return;
    }

    // Startup IPI, no shorthand
    WriteICR((6u << 8) | (startup_vec >> 12), apic_id);
}

void LApic::SendIpi(mp_ipi_t ipi)
{
    if (!lapic_present) {
        return;
    }

    LTRACEF("cpu %u target apic_id %#x, ipi %u\n", arch_curr_cpu_num(), apic_id, ipi);

    uint32_t vector;
    switch (ipi) {
        case MP_IPI_GENERIC:
            vector = LAPIC_INT_GENERIC;
            break;
        case MP_IPI_RESCHEDULE:
            vector = LAPIC_INT_RESCHEDULE;
            break;
        default:
            panic("X86: unknown IPI %u\n", ipi);
    }

    // send fixed mode, level asserted, no destination shorthand interrupt
    WriteICR(vector | (1U << 14), apic_id);
}

LApic& GetLAPIC(void)
{
    return g_lapic;
}

}

static enum handler_return lapic_timer_handler(void *arg) {
    LTRACEF("cpu %u\n", arch_curr_cpu_num());

    enum handler_return ret = INT_NO_RESCHEDULE;
    if (t_callback) {
        ret = t_callback(callback_arg, current_time());
    }

    return ret;
}

static enum handler_return lapic_spurious_handler(void *arg)  {
    LTRACEF("cpu %u, arg %p\n", arch_curr_cpu_num(), arg);

    return INT_NO_RESCHEDULE;
}

static enum handler_return lapic_generic_handler(void *arg)  {
    LTRACEF("cpu %u, arg %p\n", arch_curr_cpu_num(), arg);

    return INT_NO_RESCHEDULE;
}

static enum handler_return lapic_reschedule_handler(void *arg)  {
    LTRACEF("cpu %u, arg %p\n", arch_curr_cpu_num(), arg);

    return mp_mbx_reschedule_irq();
}

static void lapic_init_percpu(uint level) {
    kernel::arch::GetLAPIC().InitPerCpu(level);
}
LK_INIT_HOOK_FLAGS(lapic_init_percpu, lapic_init_percpu, LK_INIT_LEVEL_VM, LK_INIT_FLAG_SECONDARY_CPUS);


static uint32_t lapic_read_current_tick(void) {
    return kernel::arch::GetLAPIC().ReadCurrentTick();
}

static void lapic_timer_init_percpu(uint level) {
    auto& lapic = kernel::arch::GetLAPIC();

    lapic.InitTimerPerCpu(level);
}
LK_INIT_HOOK_FLAGS(lapic_timer_init_percpu, lapic_timer_init_percpu, LK_INIT_LEVEL_VM + 1, LK_INIT_FLAG_SECONDARY_CPUS);
