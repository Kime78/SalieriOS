#ifndef INTERRUPT_DESCRIPTOR_TABLE
#define INTERRUPT_DESCRIPTOR_TABLE
#include <stdint.h>

typedef struct {
    uint16_t offsetLow; //func pointer
    uint16_t selector; //.Code thing
    uint8_t zero8; //0
    uint8_t attributes; //0x8e for interrupt gate 
    uint16_t offsetMid; //func pointer
    uint32_t offsetHigh; //func pointer
    uint32_t zero32; //0
} __attribute__((packed)) idtEntry_t;

typedef struct {
    uint16_t limit;
    uint64_t offset;
} __attribute__((packed)) idtr_t;

void load_idt();
#endif