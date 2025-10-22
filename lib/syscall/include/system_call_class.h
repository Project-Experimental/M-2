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

};