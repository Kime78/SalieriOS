#include "vga.h"
#include "stivale.h"
#include "pmm.h"
#include "vmm.h"
#include "idt.h"
void kmain(stivale_info_t *info) {

	terminal_initialize();
	terminal_writeok("GDT Initialised!");
	load_idt();
	terminal_writeok("IDT Initialised!"); //check it
	
	init_pmn(info);
	
	terminal_writestring("Welcome to SalieriOS!\n");
	//terminal_writerror("Test error!");

	pagemap_t* test;
	if(vmm_create_pagemap(test))
	{
		terminal_writeok("Pagemap Created!");
	}
	else
	{
		terminal_writerror("Failed to create Pagemap!");
	}
	vmm_setup_pages(test);

	// uint64_t fizi = 0x1000;
	// uint64_t anime = 0x2000;
	// if(vmm_map_page(test, fizi, anime, 0b11))
	// {
	// 	terminal_writeok("VMM Mapped Memory!");
	// }
	// else
	// {
	// 	terminal_writerror("Failed to map memory!");
	// }
	int volatile cum = 0;
	int volatile shit = 5000;
	//shit = shit / cum;
}