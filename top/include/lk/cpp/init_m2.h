#pragma once

#include <lk/init.h>
#include <lk/compiler.h>
#include <sys/types.h>

namespace m2
{

template<uint16_t Level, uint16_t Flags>
struct SuperInit
{
    uint16_t level = Level;
    uint16_t flags = Flags;

    virtual void Init(uint level) = 0;
};

}

#define M2_INIT_HOOK_OBJ(_name, _class) \
    LK_INIT_HOOK_FLAGS(_name, +[](uint level) { _class.Init(level); }, _class.level, _class.flags)

#define M2_INIT_HOOK(_name, _class_type) \
    static _class_type _init_class_##_class_type; \
    M2_INIT_HOOK_OBJ(_name, _init_class_##_class_type)