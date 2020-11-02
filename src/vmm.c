#include "vmm.h"
#include "pmm.h"

bool vmm_create_pagemap(pagemap_t *map)
{
    uint64_t page = pmm_alloc(4096);
    if (map == 0) {
        return false;
    }
    uint64_t *pml4 = (uint64_t *)page;
    memset(pml4, 0, 4096);
    map->pml4 = pml4;
    return true;
}