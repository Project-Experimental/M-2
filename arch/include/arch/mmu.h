/*
 * Copyright (c) 2025 Nakada Tokumei
 * Copyright (c) 2014-2016 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#if ARCH_HAS_MMU

#include <arch.h>
#include <lk/compiler.h>
#include <stdbool.h>
#include <sys/types.h>

/* to bring in definition of arch_aspace */
#include <arch/aspace.h>

#include <arch/interface/IMmu.h>

__BEGIN_CDECLS

/* flags to pass to the arch_mmu_map and arch_mmu_query routines */
#define ARCH_MMU_FLAG_CACHED            (0U<<0)
#define ARCH_MMU_FLAG_UNCACHED          (1U<<0)
#define ARCH_MMU_FLAG_UNCACHED_DEVICE   (2U<<0) /* only exists on some arches, otherwise UNCACHED */
#define ARCH_MMU_FLAG_CACHE_MASK        (3U<<0)

#define ARCH_MMU_FLAG_PERM_USER         (1U<<2)
#define ARCH_MMU_FLAG_PERM_RO           (1U<<3)
#define ARCH_MMU_FLAG_PERM_NO_EXECUTE   (1U<<4) /* supported on most, but not all arches */
#define ARCH_MMU_FLAG_NS                (1U<<5) /* supported on some arches */
#define ARCH_MMU_FLAG_INVALID           (1U<<6) /* indicates that flags are not specified */

/* arch level query of some features at the mapping/query level */

/* forward declare the per-address space arch-specific context object */
typedef struct arch_aspace arch_aspace_t;

#define ARCH_ASPACE_FLAG_KERNEL         (1U<<0)

/*
 * load a new user address space context.
 * aspace argument NULL should unload user space.
 */
void arch_mmu_context_switch(arch_aspace_t *aspace);

namespace kernel::arch
{

class Mmu : public IMmu
{
    arch_aspace_t* aspace;
public:
    Mmu(arch_aspace_t* aspace = nullptr) : aspace(aspace)
    { } 

    static bool IsSupportsNXMappings(void);
    static bool IsSupportsNSMappings(void);
    static bool IsSupportsUserASpaces(void);

    status_t Init(
        vaddr_t base,
        size_t size,
        uint flags
    ) override;

    status_t Destroy(void) override;

    int Map(
        vaddr_t vaddr, 
        paddr_t paddr, 
        uint count,
        uint flags
    ) override;

    int UnMap(
        vaddr_t vaddr,
        uint count
    ) override;

    status_t Query(
        vaddr_t vaddr,
        paddr_t& paddr,
        uint& flags
    ) override;

    void ContextSwitch(arch_aspace_t* new_space) override;

    arch_aspace_t* GetASpace(void);
    void SetASpace(arch_aspace_t* new_aspace);
};

};

__END_CDECLS

#endif

