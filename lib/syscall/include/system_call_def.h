#pragma once

#define NR_SYSCALLS 256
#define KERNEL_CODE 0x10
#define USER_CODE 0x18

extern syscall_routine_t sys_table[NR_SYSCALLS];