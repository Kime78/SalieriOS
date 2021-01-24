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

	if(vmm_setup_pages()) 
	{
		terminal_writeok("VMM was setup correctly!");
	}
	else
	{
		terminal_writerror("An error was encountered when creating the VMM");
	}
	terminal_writestring("Welcome to SalieriOS!\n");

}