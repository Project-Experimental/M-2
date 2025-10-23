#include <system_call.h>
#include <system_call_def.h>
#include <system_call_class.h>

#include <sys/types.h>
#include <lk/compiler.h>

#include <stdio.h>

class SysCallWrite 
    : public m2::SuperSyscall<NR_WRITE>
{
public:
    int Handler(
        uint64_t a,
        uint64_t b,
        uint64_t c,
        uint64_t d,
        uint64_t e,
        uint64_t f
    )
    {
        printf("M2_SYS::->Handler Called\n");

        return 0;
    }
};

M2_SYSCALL_REGISTER_INIT(SysCallWrite);