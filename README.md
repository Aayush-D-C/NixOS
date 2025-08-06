# NixOS - A Simple Operating System

![NixOS](https://img.shields.io/badge/NixOS-v1.0-green)
![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen)
![License](https://img.shields.io/badge/License-MIT-blue)

A fully functional, custom operating system built from scratch with a beautiful fullscreen green interface and complete command-line shell.

<img width="716" height="400" alt="home" src="https://github.com/user-attachments/assets/9e19a190-0c63-4286-b857-0b4286c056a5" />

<img width="715" height="396" alt="help" src="https://github.com/user-attachments/assets/8085e9c0-60b9-4301-97b6-b00998378aa5" />

<img width="717" height="393" alt="echo" src="https://github.com/user-attachments/assets/4a017e09-e42c-4d33-bdbd-e48397c667ce" />




## 🌟 Features

###  Working Features
- **Complete Boot Process**: From bootloader to kernel execution
- **Fullscreen Green Interface**: Beautiful VGA text mode with green background and white text
- **Command-Line Shell**: Fully functional shell with command processing
- **Built-in Commands**: `help`, `clear`, `echo`, `version`, `uptime`, `reboot`
- **Proper Cursor Handling**: Normal cursor behavior and positioning
- **Protected Mode**: 16-bit to 32-bit transition with proper GDT setup
- **Keyboard Input**: Full keyboard support with scancode to ASCII mapping
- **Error Handling**: Comprehensive error messages and debugging

### 🎨 Visual Design
- **Fullscreen Green Background**: Consistent green background throughout
- **Centered Text**: All messages and commands are properly centered
- **White Text**: High contrast white text on green background
- **Professional Layout**: Organized command output and user interface

## 🚀 Quick Start

### Prerequisites
- **WSL (Windows Subsystem for Linux)** or Linux environment
- **NASM** (Netwide Assembler)
- **GCC** (GNU Compiler Collection)
- **QEMU** (for emulation)

### Installation
```bash
# Clone the repository
git clone <repository-url>
cd complete_os

# Build the OS
make clean
make

# Run in QEMU
make run
```

### Available Commands
Once the OS boots, you can use these commands:

| Command | Description |
|---------|-------------|
| `help` | Show all available commands |
| `clear` | Clear the screen (resets to green background) |
| `echo <text>` | Echo text back (centered on screen) |
| `version` | Show NixOS version information |
| `uptime` | Show system uptime message |
| `reboot` | Restart the system |

## 📁 Project Structure

```
complete_os/
├── boot.asm          # Bootloader (16-bit assembly)
├── kernel_entry.asm  # Kernel entry point (32-bit assembly)
├── kernel.c          # Main kernel code (C)
├── linker.ld         # Linker script
├── Makefile          # Build system
├── README.md         # This documentation
└── BEFORE_YOU_BEGIN.txt  # Development journey and troubleshooting
```

## 🔧 Technical Details

### Memory Layout
- **Bootloader**: `0x7C00` (512 bytes)
- **Kernel**: `0x1000` (12KB allocated)
- **Stack**: `0x90000`
- **VGA Memory**: `0xB8000`

### Build Process
1. **Bootloader**: NASM assembles `boot.asm` to `boot.bin`
2. **Kernel Entry**: NASM assembles `kernel_entry.asm` to `kernel_entry.o`
3. **Kernel**: GCC compiles `kernel.c` to `kernel.o`
4. **Linking**: LD links objects with `linker.ld` to `kernel.bin`
5. **Image Creation**: Creates `nixos.img` floppy disk image

### Key Components

#### Bootloader (`boot.asm`)
- 16-bit real mode initialization
- Disk loading (24 sectors = 12KB)
- Protected mode transition
- GDT (Global Descriptor Table) setup
- Error handling with hex codes

#### Kernel Entry (`kernel_entry.asm`)
- 32-bit protected mode entry
- Stack initialization
- Direction flag clearing
- Kernel main function call

#### Kernel (`kernel.c`)
- VGA text mode graphics system
- Keyboard input handling
- Command parsing and execution
- Fullscreen green background
- Centered text printing

## 🛠️ Development Journey

### Initial Problems
- **Kernel Loading Issue**: Bootloader only loaded 8 sectors (4KB) for 10.5KB kernel
- **Blinking Cursor**: Cursor blinking in wrong direction
- **Background Override**: Commands created black background areas
- **Protected Mode Issues**: GDT setup and transition problems

### Solutions Implemented
1. **Increased kernel loading** from 8 to 24 sectors (4KB → 12KB)
2. **Fixed cursor handling** with proper VGA register control
3. **Maintained green background** in all command functions
4. **Improved GDT setup** with proper descriptors
5. **Enhanced error handling** with detailed messages

## 🔍 Troubleshooting

### Common Issues

#### OS Won't Boot
```bash
# Check kernel size
wsl ls -la kernel.bin

# Verify sectors loaded (should be 21+ for 10.5KB kernel)
# Check bootloader error messages
```

#### Build Failures
```bash
# Check toolchain installation
which nasm
which gcc
which qemu-system-i386

# Clean and rebuild
make clean
make
```

#### Display Issues
- Verify VGA memory access (`0xB8000`)
- Check color attribute calculations
- Test fullscreen background function

### Debug Commands
```bash
# Build with debugging
make debug

# Check file sizes
ls -la *.bin *.img

# Verify image creation
file nixos.img
```

## 🎯 Technical Achievements

### Low-Level Systems Programming
- **Assembly Language**: Bootloader and kernel entry in x86 assembly
- **Protected Mode**: 16-bit to 32-bit transition
- **Memory Management**: Proper memory layout and stack setup
- **Hardware Interaction**: VGA graphics, keyboard input, disk I/O

### Operating System Concepts
- **Boot Process**: Complete bootloader to kernel chain
- **Memory Layout**: Proper segmentation and addressing
- **Device Drivers**: VGA text mode and keyboard handling
- **Command System**: Shell implementation with command parsing

### Development Skills
- **Cross-Platform Development**: WSL compatibility
- **Build Systems**: Makefile automation
- **Error Handling**: Comprehensive debugging and error messages
- **Documentation**: Complete technical documentation

## 🔮 Future Enhancements

### Planned Features
- [ ] File system support
- [ ] Multi-tasking capabilities
- [ ] Network stack implementation
- [ ] Graphics mode support
- [ ] Sound system
- [ ] More sophisticated shell
- [ ] System calls and API
- [ ] Device drivers
- [ ] Memory management
- [ ] Process management

### Possible Improvements
- [ ] GUI support
- [ ] Network connectivity
- [ ] File editing capabilities
- [ ] Package management
- [ ] User management
- [ ] Security features

## 📚 Learning Resources

### OS Development
- [OSDev Wiki](https://wiki.osdev.org/)
- [Bran's Kernel Development Tutorial](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-manual-325462.html)

### Assembly and C
- [NASM Documentation](https://www.nasm.us/doc/)
- [GCC Manual](https://gcc.gnu.org/onlinedocs/)
- [x86 Assembly Guide](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html)

## 🤝 Contributing

This project demonstrates OS development concepts and serves as a learning resource. Contributions are welcome!

### Development Guidelines
1. Test incrementally
2. Monitor kernel size
3. Use proper memory layout
4. Implement error handling
5. Document changes

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- **OSDev Community**: For excellent documentation and tutorials
- **QEMU Team**: For the excellent emulation platform
- **GNU Toolchain**: For the powerful development tools
- **Intel**: For the x86 architecture documentation

## 📞 Support

If you encounter issues or have questions:
1. Check the `BEFORE_YOU_BEGIN.txt` file for troubleshooting
2. Review the build process and error messages
3. Verify toolchain installation
4. Test on different environments

---

**NixOS v1.0** - Where it all begins! 🌟

*Built with love and assembly language* 
