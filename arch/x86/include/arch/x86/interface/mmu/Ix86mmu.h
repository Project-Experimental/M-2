#pragma once

#include "../IInitialize.h"

namespace kernel::arch
{

struct IX86mmu : IInitalize 
{
    virtual void InitEarly(void) = 0;
    virtual void InitEarlyPerCpu(void) = 0;
};

}