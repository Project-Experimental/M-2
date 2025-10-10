#pragma once

#include "../IInitialize.h"


namespace kernel::arch
{

struct ILApic : IInitalize
{
    virtual void InitPostVM() = 0;
    virtual status_t InitTimer(bool tsc_supported) = 0;
    
    virtual void Eoi(uint32_t vector) = 0;

    virtual void EnableOnLocalCPU(void) = 0;
    virtual uint32_t GetAPICId(void) = 0;

    virtual status_t SetOneShotTimer(platform_timer_callback callback, void *arg, lk_time_t interval) = 0;
    virtual void CancelTimer(void) = 0;
};

};