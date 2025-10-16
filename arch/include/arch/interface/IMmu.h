/* 
 *                  IMmu.h
 * 
 * Desrciption : Interface for Mmu class
 * 
 * Writer : Nakada Tokumei <nakada_tokumei@protonmail.com>
 * 
 * Copyright (c) 2025 Nakada Tokumei
 * 
 */


#pragma once

#include <arch.h>
#include <lk/compiler.h>
#include <sys/types.h>

#include <arch/aspace.h>

namespace kernel::arch
{

struct IMmu
{
    using arch_aspace_t = struct arch_aspace;

    // Initalizer per address Space
    virtual status_t Init(
        vaddr_t base,
        size_t size,
        uint flags
    ) = 0;

    virtual status_t Destroy(void) = 0;
    
    // Routines to map/unmap/query mappings per address space
    virtual int Map(
        vaddr_t vaddr, 
        paddr_t paddr, 
        uint count,
        uint flags
    ) = 0;

    virtual int UnMap(
        vaddr_t vaddr,
        uint count
    ) = 0;

    virtual status_t Query(
        vaddr_t vaddr,
        paddr_t& paddr,
        uint& flags
    ) = 0;

    // Load a new user address space context
    virtual void ContextSwitch(arch_aspace_t* new_space) = 0;
};

};
