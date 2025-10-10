#pragma once

#include <sys/types.h>

namespace kernel::arch
{
struct IInitalize
{
    virtual ~IInitalize() = default;
    virtual void Init(void) = 0;
};
};