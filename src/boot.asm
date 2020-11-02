section .stivalehdr

stivale_header:
    dq stack.top    ; rsp
    dw 0          ; video mode
    dw 0            ; fb_width
    dw 0            ; fb_height
    dw 0            ; bpp
    dq 0

section .data

global GDT64

; The GDT (Global descriptor table)
GDT64:
    .Null: equ $ - GDT64
    dq 0
    .Code: equ $ - GDT64 ;0 8 16 24 32 and so on
    dw 0
    dw 0
    db 0
    db 10011010b
    db 00100000b
    db 0
    .Data: equ $ - GDT64
    dw 0
    dw 0
    db 0
    db 10010010b
    db 00000000b
    db 0
GDT_END:

GDT_PTR:
    dw GDT_END - GDT64 - 1    ; Limit
    dq GDT64                  ; Base
    
section .bss

stack:
    resb 4096
  .top:

section .text
extern kmain
global exec_start ; So qloader2 can find the entry point properly
exec_start:
    lgdt [GDT_PTR]

    ; Push the values for iretq
    mov rax, rsp ; Save before we push
    push 0x10       ; ss
    push rax        ; rsp
    pushf           ; rflags
    push 0x8        ; cs
    push run_kernel ; rip
    iretq ; "Return" to the run_kernel
    
run_kernel:
    mov ax, 0x10 ; We can't write to the segment registers directly

    mov ds, ax
    mov es, ax
    mov ss, ax
    mov gs, ax
    mov fs, ax

    ; Run the kernel
    call kmain

    ; Halt if kmain returns
    cli
    hlt