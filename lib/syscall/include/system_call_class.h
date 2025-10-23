#pragma once

#include <stdint.h>

#include <system_call.h>
#include <system_call_def.h>

namespace m2
{

template<uint8_t Num>
struct SuperSyscall
{
    uint8_t syscall_num = Num;
    virtual int Handler(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = 0;
};

#define M2_SYSCALL_REGISTER_ARRAY(_class) \
    sys_table[_class.syscall_num] = +[](uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f) -> int { return _class.Handler(a, b, c, d, e, f); }

#define M2_SYSCALL_REGISTER_INIT(__class_type)                  \
    static __class_type _init_class_##__class_type;             \
    static void __syscall_init_func_##__class_type(void)        \
    {                                                           \
        M2_SYSCALL_REGISTER_ARRAY(_init_class_##__class_type);  \
    }                                                           \
    syscall_init_t __syscall_init_##__class_type __attribute__((section(".syscall.init"))) = &__syscall_init_func_##__class_type

};