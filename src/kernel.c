#include "vga.h"
#include "stivale.h"
#include "pmm.h"
#include <idt.h>
void kmain(stivale_info_t *info) {

	terminal_initialize();
	terminal_writestring("[  OK  ] GDT Initialised!\n"); //check it
	load_idt();
	terminal_writestring("[  OK  ] IDT Initialised!\n"); //check it
	
	init_pmn(info);
	
	terminal_writestring("Welcome to SalieriOS!\n");
	
	volatile int cum = 0;
	volatile int shit = 500 / cum;

}