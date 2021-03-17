#ifndef PHYSICAL_MEMORY_MANAGER
#define PHYSICAL_MEMORY_MANAGER
#include "stivale.h"
#define KERNEL_PHYS_OFFSET ((size_t)0xffffffff80000000)
#define MEM_PHYS_OFFSET ((size_t)0xffff800000000000)

void init_pmn(stivale_info_t *info);
void pmm_set_page_used(uint64_t page);
void pmm_set_page_free(uint64_t page);
uint8_t is_page_used(uint64_t page);
uint64_t pmm_find_free_pages(uint64_t size);
void *pmm_alloc(uint64_t size);
void pmm_unalloc(void *addr, uint64_t size);

#endif