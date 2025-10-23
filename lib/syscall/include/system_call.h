#pragma once

#include <lk/compiler.h>
#include <lk/cpp/init_m2.h>

#include <stdio.h>

#include <system_call_def.h>

typedef void (*syscall_init_t)(void);
typedef void (*syscall_init_arr_t[])(void);
typedef int (*syscall_routine_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

__BEGIN_CDECLS

int syscall_entry(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

__END_CDECLS

extern syscall_routine_t sys_table[NR_SYSCALLS];

