// NixOS Kernel (kernel.c)

#include <stdint.h>
#include <stddef.h>

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Colors
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN 14
#define VGA_COLOR_WHITE 15

// Keyboard constants
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Global variables
static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color = 7;
static uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;
static char command_buffer[256];
static size_t command_index = 0;

// I/O functions
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Cursor functions
void update_cursor(void) {
    uint16_t pos = terminal_row * VGA_WIDTH + terminal_column;
    outb(0x3D4, 14);
    outb(0x3D5, (pos >> 8) & 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void hide_cursor(void) {
    outb(0x3D4, 14);
    outb(0x3D5, 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, 0xFF);
}

void show_cursor(void) {
    update_cursor();
}

// String functions
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(unsigned char*)a - *(unsigned char*)b;
}

void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Terminal functions
void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = ((uint16_t)terminal_color << 8) | ' ';
        }
    }
    terminal_row = 0;
    terminal_column = 0;
}

void terminal_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ((uint16_t)terminal_color << 8) | ' ';
    }
    terminal_row = VGA_HEIGHT - 1;
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    } else {
        const size_t index = terminal_row * VGA_WIDTH + terminal_column;
        terminal_buffer[index] = ((uint16_t)terminal_color << 8) | c;
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_scroll();
            }
        }
    }
    update_cursor();
}

void terminal_print(const char* data) {
    for (size_t i = 0; i < strlen(data); i++) {
        terminal_putchar(data[i]);
    }
}

// New function to set fullscreen green background
void set_fullscreen_green(void) {
    uint8_t green_bg_white_fg = (VGA_COLOR_GREEN << 4) | VGA_COLOR_WHITE;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = ((uint16_t)green_bg_white_fg << 8) | ' ';
        }
    }
}

// New function to print centered text
void print_centered(const char* text, size_t row) {
    size_t text_len = strlen(text);
    size_t start_col = (VGA_WIDTH - text_len) / 2;
    
    // Set cursor position
    terminal_row = row;
    terminal_column = start_col;
    
    // Print the text
    terminal_print(text);
}

// Shell commands
void cmd_help(void) {
    // Set green background for help output
    set_fullscreen_green();
    
    // Print centered help header
    print_centered("NixOS Commands:", 8);
    
    // Print commands with proper positioning
    terminal_row = 10;
    terminal_column = 20;
    terminal_print("help    - Show this help message");
    
    terminal_row = 11;
    terminal_column = 20;
    terminal_print("clear   - Clear the screen");
    
    terminal_row = 12;
    terminal_column = 20;
    terminal_print("echo    - Echo text back");
    
    terminal_row = 13;
    terminal_column = 20;
    terminal_print("uptime  - Show system uptime");
    
    terminal_row = 14;
    terminal_column = 20;
    terminal_print("version - Show OS version");
    
    terminal_row = 15;
    terminal_column = 20;
    terminal_print("reboot  - Restart the system");
    
    // Move cursor back to input area
    terminal_row = 20;
    terminal_column = 0;
}

void cmd_clear(void) {
    set_fullscreen_green();
    terminal_row = 0;
    terminal_column = 0;
}

void cmd_echo(const char* args) {
    if (strlen(args) > 0) {
        // Set green background and print centered
        set_fullscreen_green();
        print_centered(args, 12);
        terminal_row = 20;
        terminal_column = 0;
    }
}

void cmd_uptime(void) {
    // Set green background and print centered
    set_fullscreen_green();
    print_centered("System has been running since boot", 12);
    terminal_row = 20;
    terminal_column = 0;
}

void cmd_version(void) {
    // Set green background and print centered
    set_fullscreen_green();
    print_centered("NixOS v1.0", 12);
    print_centered("Where it all begins!", 13);
    terminal_row = 20;
    terminal_column = 0;
}

void cmd_reboot(void) {
    // Set green background and print centered
    set_fullscreen_green();
    print_centered("Rebooting system...", 12);
    terminal_row = 20;
    terminal_column = 0;
    // Small delay before reboot
    for (volatile int i = 0; i < 1000000; i++) {}
    // Triple fault to reboot
    asm volatile("int $0x03");
}

// Command processing
void process_command(const char* cmd) {
    if (strlen(cmd) == 0) {
        return;
    }
    
    // Parse command and arguments
    char command[64] = {0};
    char args[192] = {0};
    
    size_t i = 0;
    // Extract command
    while (cmd[i] && cmd[i] != ' ' && i < 63) {
        command[i] = cmd[i];
        i++;
    }
    command[i] = '\0';
    
    // Extract arguments
    if (cmd[i] == ' ') {
        i++; // Skip space
        size_t j = 0;
        while (cmd[i] && j < 191) {
            args[j++] = cmd[i++];
        }
        args[j] = '\0';
    }
    
    // Execute commands
    if (strcmp(command, "help") == 0) {
        cmd_help();
    } else if (strcmp(command, "clear") == 0) {
        cmd_clear();
    } else if (strcmp(command, "echo") == 0) {
        cmd_echo(args);
    } else if (strcmp(command, "uptime") == 0) {
        cmd_uptime();
    } else if (strcmp(command, "version") == 0) {
        cmd_version();
    } else if (strcmp(command, "reboot") == 0) {
        cmd_reboot();
    } else {
        terminal_print("Unknown command: ");
        terminal_print(command);
        terminal_print("\nType 'help' for available commands.\n");
    }
}

// Keyboard handling
char get_keyboard_input(void) {
    static uint8_t key_pressed[256] = {0};
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Key release (bit 7 set)
    if (scancode & 0x80) {
        key_pressed[scancode & 0x7F] = 0;
        return 0;
    }
    
    // Key press
    if (!key_pressed[scancode]) {
        key_pressed[scancode] = 1;
        
        // Simple scancode to ASCII mapping
        static const char scancode_to_ascii[] = {
            0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
            '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
            0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
            0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
            '*', 0, ' '
        };
        
        if (scancode < sizeof(scancode_to_ascii)) {
            return scancode_to_ascii[scancode];
        }
    }
    return 0;
}

// Shell loop
void shell(void) {
    // Set green background for shell
    set_fullscreen_green();
    
    // Print centered welcome message
    print_centered("NixOS Shell v1.0", 2);
    print_centered("Type 'help' for available commands.", 4);
    print_centered("", 6); // Empty line
    
    // Set cursor to bottom area for input
    terminal_row = 20;
    terminal_column = 0;
    
    while (1) {
        // Move cursor to input area
        terminal_row = 20;
        terminal_column = 0;
        terminal_print("$ ");
        command_index = 0;
        
        // Read command
        while (1) {
            char c = get_keyboard_input();
            if (c == 0) continue;
            
            if (c == '\n') {
                terminal_putchar('\n');
                command_buffer[command_index] = '\0';
                process_command(command_buffer);
                break;
            } else if (c == '\b') {
                if (command_index > 0) {
                    command_index--;
                    // Move cursor back and clear character
                    if (terminal_column > 0) {
                        terminal_column--;
                    } else if (terminal_row > 0) {
                        terminal_row--;
                        terminal_column = VGA_WIDTH - 1;
                    }
                    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
                    terminal_buffer[index] = ((uint16_t)((VGA_COLOR_GREEN << 4) | VGA_COLOR_WHITE) << 8) | ' ';
                }
            } else if (c >= 32 && c <= 126 && command_index < 255) {
                command_buffer[command_index++] = c;
                terminal_putchar(c);
            }
        }
    }
}

// Kernel entry point
void kernel_main(void) {
    // Set fullscreen green background
    set_fullscreen_green();
    
    // Print centered startup messages
    print_centered("NixOS Kernel Starting...", 10);
    print_centered("Welcome to NixOS!", 12);
    print_centered("==================", 13);
    
    // Small delay for visual effect
    for (volatile int i = 0; i < 500000; i++) {}
    
    // Start shell
    shell();
    
    // Should never reach here
    while (1) {
        asm volatile("hlt");
    }
}