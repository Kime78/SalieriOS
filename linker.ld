ENTRY(exec_start)

OUTPUT_FORMAT(elf64-x86-64)

KRNL_VMA_START = 0xFFFFFFFF80000000;

SECTIONS
{
    . = KRNL_VMA_START + 0x100000;

    .stivalehdr : ALIGN(4K) {
        KEEP(*(.stivalehdr))
    }

    .text : ALIGN(4K) {
        KEEP(*(.text*))
    }

    .rodata : ALIGN(4K) {
        KEEP(*(.rodata*))
    }

    .data : ALIGN(4K) {
        KEEP(*(.data*))
    }

    .bss : ALIGN(4K) {
        KEEP(*(.bss*))
        KEEP(*(COMMON))
    }
}