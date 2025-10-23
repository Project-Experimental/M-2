    #include <system_call_def.h>

    .extern syscall_table
    
    .global syscall_entry
    .type syscall_entry, @function
syscall_entry:
    swapgs
    push %rcx
    push %r11
    
    mov %r10, %rcx
    cmp $NR_SYSCALLS, %rax
    jae .bad_syscall
    call *sys_table(,%rax,8)
    jmp .ret

.bad_syscall:
    # Return ENOSYS
    mov $-38, %rax

.ret:
    pop %r11
    pop %rcx
    swapgs
    sysretq

    .size syscall_entry, . - syscall_entry