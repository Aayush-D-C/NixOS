; Simple OS Bootloader (boot.asm)
; This bootloader loads our kernel and jumps to it

[BITS 16]
[ORG 0x7C00]

start:
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Print boot message
    mov si, boot_msg
    call print_string

    ; Reset disk system first
    mov ah, 0x00        ; Reset disk function
    mov dl, 0x00        ; Drive 0 (floppy)
    int 0x13            ; BIOS disk interrupt

    ; Load kernel from disk - increased sectors to 24 (12KB)
    mov ah, 0x02        ; Read sectors function
    mov al, 24          ; Number of sectors to read (increased to 24 for 12KB)
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Sector 2 (sector 1 is bootloader)
    mov dh, 0           ; Head 0
    mov dl, 0x00        ; Drive 0 (floppy disk, not hard disk)
    mov bx, 0x1000      ; Load kernel at 0x1000
    int 0x13            ; BIOS disk interrupt

    jc disk_error       ; Jump if error

    ; Print success message
    mov si, load_success_msg
    call print_string

    ; Switch to protected mode
    cli                 ; Disable interrupts
    
    ; Load GDT
    lgdt [gdt_descriptor]
    
    ; Set PE bit in CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Jump to 32-bit code
    jmp 0x08:protected_mode

disk_error:
    mov si, disk_error_msg
    call print_string
    
    ; Show error details
    mov si, error_code_msg
    call print_string
    mov al, ah          ; Error code is in AH
    call print_hex
    mov si, newline_msg
    call print_string
    
    jmp $               ; Infinite loop instead of halt

print_hex:
    ; Print AL as hex
    push ax
    shr al, 4           ; Upper nibble
    call print_nibble
    pop ax
    and al, 0x0F        ; Lower nibble
    call print_nibble
    ret

print_nibble:
    cmp al, 9
    jle .digit
    add al, 7           ; Convert A-F
.digit:
    add al, '0'
    mov ah, 0x0E
    int 0x10
    ret

print_string:
    mov ah, 0x0E
.next_char:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .next_char
.done:
    ret

[BITS 32]
protected_mode:
    ; Set up data segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Jump to kernel
    jmp 0x1000

; Global Descriptor Table
gdt_start:
    ; Null descriptor
    dq 0

    ; Code descriptor (0x08)
    dw 0xFFFF       ; Limit
    dw 0x0000       ; Base (low)
    db 0x00         ; Base (middle)
    db 10011010b    ; Access byte (present, ring 0, code, readable)
    db 11001111b    ; Granularity (4KB pages, 32-bit)
    db 0x00         ; Base (high)

    ; Data descriptor (0x10)
    dw 0xFFFF       ; Limit
    dw 0x0000       ; Base (low)
    db 0x00         ; Base (middle)
    db 10010010b    ; Access byte (present, ring 0, data, writable)
    db 11001111b    ; Granularity (4KB pages, 32-bit)
    db 0x00         ; Base (high)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

boot_msg db 'NixOS Bootloader v1.0', 13, 10, 'Loading kernel...', 13, 10, 0
disk_error_msg db 'Disk read error! Code: ', 0
error_code_msg db '0x', 0
newline_msg db 13, 10, 0
load_success_msg db 'Kernel loaded successfully!', 13, 10, 0

times 510-($-$$) db 0
dw 0xAA55