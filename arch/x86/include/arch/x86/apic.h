/*
 * Copyright (c) 2025 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/timer.h>
#include <stdbool.h>
#include <sys/types.h>
#include <kernel/mp.h>
#include <lib/fixed_point.h>

#include "interface/apic/ILapic.h"
#include "interface/apic/ILpicSend.h"

// local apic registers
enum lapic_regs {
    LAPIC_ID = 0x20,
    LAPIC_VERSION = 0x30,
    LAPIC_TPR = 0x80,
    LAPIC_APR = 0x90,
    LAPIC_PPR = 0xa0,
    LAPIC_EOI = 0xb0,
    LAPIC_RRD = 0xc0,
    LAPIC_LDR = 0xd0,
    LAPIC_DFR = 0xe0,
    LAPIC_SVR = 0xf0,
    LAPIC_ISR0 = 0x100,

    LAPIC_TMR0 = 0x180,

    LAPIC_IRR0 = 0x200,

    LAPIC_ESR = 0x280,

    LAPIC_CMCI = 0x2f0,
    LAPIC_ICRLO = 0x300,
    LAPIC_ICRHI = 0x310,
    LAPIC_TIMER = 0x320,
    LAPIC_THERMAL = 0x330,
    LAPIC_PERF = 0x340,
    LAPIC_LINT0 = 0x350,
    LAPIC_LINT1 = 0x360,
    LAPIC_ERROR = 0x370,
    LAPIC_TICR = 0x380,
    LAPIC_TCCR = 0x390,
    LAPIC_DIV = 0x3e0,

    // Extended features
    LAPIC_EXT_FEATURES = 0x400,
    LAPIC_EXT_CONTROL = 0x410,
    LAPIC_EXT_SEOI = 0x420,
    LAPIC_EXT_IER0 = 0x480,
    LAPIC_EXT_LVT0 = 0x500,
};

enum lapic_interrupts {
    LAPIC_INT_TIMER = 0xf8,
    LAPIC_INT_GENERIC,
    LAPIC_INT_RESCHEDULE,

    LAPIC_INT_SPURIOUS = 0xff, // Bits 0-3 must be 1 for P6 and below compatibility
};

enum lapic_timer_mode {
    LAPIC_TIMER_MODE_ONESHOT = 0,
    LAPIC_TIMER_MODE_PERIODIC = 1,
    LAPIC_TIMER_MODE_TSC_DEADLINE = 2,
};

// local apic
namespace kernel::arch
{

class LApic : public ILApic, public ILApicSend
{
    uint32_t apic_id;

    bool lapic_present;
    bool lapic_x2apic;
    bool use_tsc_deadline;
    volatile uint32_t *lapic_mmio;
    struct fp_32_64 timebase_to_lapic;

private:
    uint32_t Read(enum lapic_regs reg);
    void Write(enum lapic_regs reg, uint32_t val);
    void WaitForICRDelivery(void);
    void WriteICR(uint32_t low, uint32_t apic_id);

public:
    void Init(void);
    void InitPerCpu(uint level);
    void InitPostVM(void);
    status_t InitTimer(bool tsc_supported);
    void InitTimerPerCpu(uint level);
    
    void Eoi(uint32_t vector);

    void EnableOnLocalCPU(void);
    uint32_t GetAPICId(void);
    void SetAPICId(uint32_t apic_id);

    status_t SetOneShotTimer(platform_timer_callback callback, void *arg, lk_time_t interval);
    void CancelTimer(void);

    void SendInitIpi(bool level);
    void SendStartupIpi(uint32_t startup_vec);
    void SendIpi(mp_ipi_t ipi);

    uint32_t ReadCurrentTick(void);
};

LApic& GetLAPIC(void);

};

// io apic(s)
status_t ioapic_init(int index,paddr_t phys_addr, uint apic_id, uint gsi_base);
