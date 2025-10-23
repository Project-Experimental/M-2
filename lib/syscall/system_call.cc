#include <stdint.h>

#include <arch/x86.h>
#include <system_call.h>
#include <system_call_def.h>

#define KERNEL_PRINTF(...) printf("M2::->" __VA_ARGS__)

syscall_routine_t sys_table[NR_SYSCALLS] __attribute__((section(".syscall.m2")));

extern syscall_init_arr_t __syscall_handler_init_start;
extern syscall_init_arr_t __syscall_handler_init_end;

class SysCallInit 
    : public m2::SuperInit<LK_INIT_LEVEL_ARCH, LK_INIT_FLAG_PRIMARY_CPU>
{
    void EnableSyscall()
    {
        uint64_t efer = read_msr(X86_MSR_IA32_EFER);
        // SCE bit enable
        efer |= 0x01;
        write_msr(X86_MSR_IA32_EFER, efer);
    }

    void SetUserKernelCode(
        uint64_t kernel_code, 
        uint64_t user_code)
    {
        uint64_t star = (user_code << 48) | (kernel_code < 32);
        write_msr(X86_MSR_IA32_STAR, star);
    }

    void SetSyscallHandler(
        syscall_routine_t sys_routine
    )
    {
        write_msr(X86_MSR_IA32_LSTAR, reinterpret_cast<uint64_t>(sys_routine));
    }

    void SyscallTableInit(void)
    {
        auto count = __syscall_handler_init_end - __syscall_handler_init_start;

        KERNEL_PRINTF("Syscall Init Count : %d\n", count);
        
        auto i = 0;
        for (; i < count; i++)
            __syscall_handler_init_start[i]();
    }

public:
    void Init(uint level)
    {
        KERNEL_PRINTF("System Call Init Enterd\n");

        EnableSyscall();
        SetUserKernelCode(KERNEL_CODE, USER_CODE);
        SetSyscallHandler(syscall_entry);
        SyscallTableInit();
    }
};

M2_INIT_HOOK(x86_systemcall, SysCallInit);
