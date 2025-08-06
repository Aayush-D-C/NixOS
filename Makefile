# Makefile for NixOS

# Compiler and tools
CC = gcc
AS = nasm
LD = ld

# Flags
CFLAGS = -m32 -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

# Files
BOOT_SRC = boot.asm
KERNEL_ENTRY_SRC = kernel_entry.asm
KERNEL_SRC = kernel.c
KERNEL_ENTRY_OBJ = kernel_entry.o
KERNEL_OBJ = kernel.o
BOOT_BIN = boot.bin
KERNEL_BIN = kernel.bin
OS_IMG = nixos.img

# Default target
all: $(OS_IMG)

# Build bootloader
$(BOOT_BIN): $(BOOT_SRC)
	$(AS) -f bin $(BOOT_SRC) -o $(BOOT_BIN)

# Build kernel entry object
$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY_SRC)
	$(AS) $(ASFLAGS) $(KERNEL_ENTRY_SRC) -o $(KERNEL_ENTRY_OBJ)

# Build kernel object
$(KERNEL_OBJ): $(KERNEL_SRC)
	$(CC) $(CFLAGS) -c $(KERNEL_SRC) -o $(KERNEL_OBJ)

# Link kernel
$(KERNEL_BIN): $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) linker.ld
	$(LD) $(LDFLAGS) $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) -o $(KERNEL_BIN)

# Create OS image
$(OS_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	# Create 1.44MB floppy image
	dd if=/dev/zero of=$(OS_IMG) bs=512 count=2880
	# Write bootloader to first sector
	dd if=$(BOOT_BIN) of=$(OS_IMG) bs=512 count=1 conv=notrunc
	# Write kernel starting at sector 2, increased to 24 sectors
	dd if=$(KERNEL_BIN) of=$(OS_IMG) bs=512 seek=1 count=24 conv=notrunc,sync

# Run in QEMU
run: $(OS_IMG)
	qemu-system-i386 -fda $(OS_IMG) -boot a

# Run in QEMU with debugging
debug: $(OS_IMG)
	qemu-system-i386 -fda $(OS_IMG) -boot a -s -S

# Clean build files
clean:
	rm -f $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) $(BOOT_BIN) $(KERNEL_BIN) $(OS_IMG)

# Create bootable USB (be careful with device!)
# usb: $(OS_IMG)
#	sudo dd if=$(OS_IMG) of=/dev/sdX bs=1M

.PHONY: all run debug clean