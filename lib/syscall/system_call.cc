#include <system_call.h>

class SysCallInit 
    : public m2::SuperInit<LK_INIT_LEVEL_ARCH, LK_INIT_FLAG_PRIMARY_CPU>
{
public:
    void Init(uint level)
    {
        printf("M2::-> System Call Init Enterd\n");
    }
};


M2_INIT_HOOK(x86_systemcall, SysCallInit);
