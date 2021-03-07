#include "vmm.h"
#include "pmm.h"
#include "vga.h"

#define PAGE_TABLE_ENTRIES 512
#define KERNEL_PHYS_OFFSET ((size_t)0xffffffff80000000)
#define MEM_PHYS_OFFSET ((size_t)0xffff800000000000)

uintptr_t higher_half(uintptr_t arg)
{
    return arg + KERNEL_PHYS_OFFSET;
}

bool vmm_setup_pages()
{
    static pagemap_t __kp;
    pagemap_t *kernel_map = &__kp;

    if (!vmm_create_pagemap(kernel_map))
    {
        return false;
    }

    if (kernel_map->pml4 == MEM_PHYS_OFFSET)
    {
        return false;
    }

    //map the first 32 MiB
    for (size_t i = 0; i < 0x1000000; i += 0x1000)
    {
        if (!vmm_map_page(kernel_map, higher_half(i), i, 0b11))
        {
            return false;
        }
        //uintptr_t addr = i * 0x1000;
        //vmm_map_page(kernel_map, addr, addr, 0b11);
        //vmm_map_page(kernel_map, addr, MEM_PHYS_OFFSET + addr, 0x03);
        //vmm_map_page(kernel_map, addr, KERNEL_PHYS_OFFSET + addr, 0x03 | (1 << 8));
    }
    asm volatile(
        "mov %0,%%cr3"
        :
        : "r"(kernel_map->pml4)
        : "memory");

    // char* rezultat;
    // tostring(rezultat, 2);
    // terminal_writeok(rezultat);
    // //map the first of 4 GiB
    // for (size_t i = 0; i < (0x100000000 / 0x1000); i++)
    // {
    //     size_t addr = i * 0x1000;
    //     //vmm_map_page(kernel_map, addr, MEM_PHYS_OFFSET + addr, 0x03);
    // }
    // vmm_map_page(kernel_map, kernel_map->pml4, kernel_map->pml4 - KERNEL_PHYS_OFFSET, 0b11);
    // //asm volatile ("mov %0, %%cr3" :: "r"(kernel_map->pml4 - KERNEL_PHYS_OFFSET):"memory");
    return true;
}

bool vmm_create_pagemap(pagemap_t *map)
{
    uint64_t page = pmm_alloc(4096);
    if (page == 0)
    {
        return false;
    }
    uint64_t *pml4 = (uint64_t *)page;
    memset(pml4, 0, 4096);
    map->pml4 = pml4;
    return true;
}

uint64_t *virtual_mem;

uintptr_t *walk_to_page_and_map(uint64_t *current, uint16_t index)
{
    // if(current[index] == 0)
    // {
    //     current[index] = pmm_alloc(4096);
    //     current[index] |= 0b11;
    // }

    // return current[index] & ~(0b111111111111ULL);
    uintptr_t ret;
    if (current[index] & 0x1) //if its already mapped
    {
        ret = current[index] & ~((uintptr_t)0xfff);
    }
    else
    {
        ret = (uintptr_t)pmm_alloc(1);
        if (ret == 0)
        {
            return NULL;
        }

        current[index] = ret | 0b111;
    }

    return (void *)ret;
}

bool vmm_map_page(pagemap_t *page_map, uintptr_t virt, uintptr_t phys, uintptr_t flags)
{
    uint16_t offset = virt & 0b111111111111;
    // uint16_t level1 = (virt >> 12);
    // uint16_t level2 = (virt >> 21);
    // uint16_t level3 = (virt >> 30);
    // uint16_t level4 = (virt >> 39);

    uintptr_t level4 = (virt & ((uintptr_t)0x1ff << 39)) >> 39;
    uintptr_t level3 = (virt & ((uintptr_t)0x1ff << 30)) >> 30;
    uintptr_t level2 = (virt & ((uintptr_t)0x1ff << 21)) >> 21;
    uintptr_t level1 = (virt & ((uintptr_t)0x1ff << 12)) >> 12;

    uint64_t *root = page_map->pml4;

    uint64_t *pml3 = walk_to_page_and_map(root, level4);
    if (pml3 == NULL)
    {
        ///return false;
    }
    uint64_t *pml2 = walk_to_page_and_map(pml3, level3);
    if (pml2 == NULL)
    {
        //return false;
    }
    uint64_t *pml1 = walk_to_page_and_map(pml2, level2);
    if (pml1 == NULL)
    {
        //return false;
    }

    pml1[level1] = phys | flags;

    uint64_t x = pml1[level1];

    return (x != 0);
}

bool vmm_unmap_page(pagemap_t *page_map, uintptr_t virt)
{
    uint16_t offset = virt & 0b111111111111;
    uint16_t level1 = (virt >> 12) & 0b111111111;
    uint16_t level2 = (virt >> 21) & 0b111111111;
    uint16_t level3 = (virt >> 30) & 0b111111111;
    uint16_t level4 = (virt >> 39) & 0b111111111;

    uint64_t *root = page_map->pml4;
    uint64_t *pml3 = walk_to_page_and_map(root, level4);
    uint64_t *pml2 = walk_to_page_and_map(root, level3);
    uint64_t *pml1 = walk_to_page_and_map(root, level2);

    pml1[level1] = 0;
    return true;
}
