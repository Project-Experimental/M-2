#pragma once

#include <sys/types.h>
#include <kernel/mp.h>
#include "../IInitialize.h"

namespace kernel::arch
{

struct ILApicSend
{
    virtual void SendInitIpi(bool level) = 0;
    virtual void SendStartupIpi(uint32_t startup_vec) = 0;
    virtual void SendIpi(mp_ipi_t ipi) = 0;
};

};