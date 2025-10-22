#pragma once

#include <lk/compiler.h>
#include <lk/cpp/init_m2.h>

#include <stdio.h>

typedef int (*syscall_routine_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

__BEGIN_CDECLS

int syscall_entry(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

__END_CDECLS
