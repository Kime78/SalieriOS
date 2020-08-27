#include <idt.h>
#include <vga.h>
void test_zero()
{
    terminal_writestring("[ INTR ] Divided by 0!");
    asm inline ("iretq");
}
void load_idt()
{
    idtr_t idtr;
    idtEntry_t idt[256]; //maybe this should be global ??
   
    idtr.limit = 256 * sizeof(idtEntry_t) - 1;
    idtr.offset = (uint64_t)&idt;

    
    void *zero = test_zero; //0 handler
    uint64_t val = &test_zero;
    idt[0].zero32 = 0;
    idt[0].zero8 = 0;
    idt[0].selector = 0x8;
    idt[0].attributes = 0x8E;
    idt[0].offsetHigh = val >> 32;
    idt[0].offsetMid = (val & 0xFFFF0000) >> 16;
    idt[0].offsetLow = val & 0xFFFF;

    asm volatile ("lidtq %0" :: "m"(idtr));
}
