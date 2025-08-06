; Kernel entry point (kernel_entry.asm)
[BITS 32]

extern kernel_main

section .text
global _start

_start:
    ; Set up stack properly
    mov esp, 0x90000
    mov ebp, esp
    
    ; Clear direction flag
    cld
    
    ; Call the C kernel
    call kernel_main
    
    ; If kernel returns, halt
.halt:
    cli
    hlt
    jmp .halt