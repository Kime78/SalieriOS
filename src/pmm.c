#include <stdint.h>
#include <stdbool.h>
#include <stivale.h>
#include <vga.h>

bool* bitmap;
uint64_t memory_pages;

void* memset(void* bufptr, int value, size_t size) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}

void init_pmn(stivale_info_t *info)
{
    mmap_entry_t *mmap = (void *) info->memory_map_addr; // Make the addr into a pointer that we can use like an array
    uint64_t memory_bytes = (mmap[info->memory_map_entries - 1].addr + mmap[info->memory_map_entries - 1].len); // The address of the last entry plus it's length
    memory_pages = (memory_bytes + 0x1000 - 1) / 0x1000; // Rounding up, THIS SHOULD BE GLOBAL (caps for attention lol), we need this to iterate the bitmap
    uint64_t bitmap_bytes = (memory_pages + 8 - 1) / 8;

    bitmap = (void *) 0;
    for (uint64_t i = 1; i < info->memory_map_entries; i++) {
        if (mmap[i].type == STIVALE_MEMORY_AVAILABLE) {
            if (mmap[i].len >= bitmap_bytes) { // If the space is large enough
                bitmap = (void *) (mmap[i].addr + 0xFFFF800000000000); 
                break;
            }
        }
    }

    if (!bitmap) {
        terminal_writestring("[FAILED] Could not find address to put bitmap! Halting.");
        while (1) {
            asm volatile("hlt");
        }
    }
    else
    {
        terminal_writestring("[  OK  ] Bitmap Initialised!\n");
    }
    
    
    //memset(bitmap, 0xff, bitmap_bytes); // Mark everything as unusable
    for (uint64_t i = 1; i < info->memory_map_entries; i++) {
        if (mmap[i].type == STIVALE_MEMORY_AVAILABLE) {
            if (mmap[i].addr == (uint64_t) bitmap) { // If this is where the bitmap is stored, don't wanna mark the bitmap as free
                uint64_t bitmap_end_byte = (uint64_t) bitmap + bitmap_bytes;
                uint64_t bitmap_end_page = ((bitmap_end_byte + 0x1000 - 1) / 0x1000) * 0x1000;
                uint64_t entry_end_page = (mmap[i].addr + mmap[i].len) / 0x1000; // Usable entries in stivale are guaranteed to be page aligned

                for (uint64_t page = bitmap_end_page; page < entry_end_page; page++) { // Continue until we have freed all pages
                    pmm_set_page_free(page);
                }
            } else {
                uint64_t page = mmap[i].addr / 0x1000;
                uint64_t count = mmap[i].len / 0x1000;
                
                for (uint64_t j = 0; j < count; j++) {
                    pmm_set_page_free(page + j);
                }
            }
        }
    }
    terminal_writestring("[  OK  ] PMM initialised!\n");
}

void pmm_set_page_used(uint64_t page) {
    uint64_t byte = page / 8;
    uint64_t bit = page % 8;
    bitmap[byte] |= (1<<bit);
}

void pmm_set_page_free(uint64_t page) {
    uint64_t byte = page / 8;
    uint64_t bit = page % 8;
    bitmap[byte] &= ~(1<<bit);
}

uint8_t is_page_used(uint64_t page) {
    uint64_t byte = page / 8;
    uint64_t bit = page % 8;
    return (bitmap[byte] & (1<<bit)) >> bit;
}

/* This function iterates through all pages, and checks if they are used. If they are not, it sets the current_page (the return value) if there are not any pages that have been found so far, and if it finds a used page, it resets the found_pages counter */
uint64_t pmm_find_free_pages(uint64_t size) {
    uint64_t needed_pages = (size + 0x1000 - 1) / 0x1000;
    uint64_t found_pages = 0;
    uint64_t current_page = 0;
    for (uint64_t i = 0; i < memory_pages; i++) {
        if (!is_page_used(i)) {
            if (found_pages == 0) {
                current_page = i;
            }
            found_pages++;
        } else {
            found_pages = 0;
        }

        if (found_pages >= needed_pages) {
            return current_page;
        }
    }

    terminal_writestring("[FAILED] Free memory not found!");
    while (1) {
        asm volatile("hlt");
    }
}

/* This function uses the find_free_pages function to find a chunk of pages, then marks them as allocated */
void *pmm_alloc(uint64_t size) {
    uint64_t needed_pages = (size + 0x1000 - 1) / 0x1000;
    uint64_t free_page = pmm_find_free_pages(size);

    for (uint64_t i = 0; i < needed_pages; i++)  {
        pmm_set_page_used(free_page + i);
    }
}

/* This function just marks whatever pages free that are passed to it */
void pmm_unalloc(void *addr, uint64_t size) {
    uint64_t page = (uint64_t) addr / 0x1000;
    uint64_t pages = (size + 0x1000 - 1) / 0x1000;

    for (uint64_t i = 0; i < pages; i++) {
        pmm_set_page_free(page + i);
    }
}