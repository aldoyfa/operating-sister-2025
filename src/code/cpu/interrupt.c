#include "../../header/cpu/portio.h"
#include "../../header/cpu/interrupt.h"
#include "../../header/driver/keyboard.h"
#include "../../header/text/framebuffer.h"
#include "../../header/filesystem/fat32.h"
#include "../../header/cpu/gdt.h"
#include "../../header/text/terminaltext.h"
#include "../../header/process/scheduler.h"
#include "../../header/stdlib/string.h"
#include "../../header/driver/cmos.h"

// Forward declarations for functions that are implemented in usermode
// These should be replaced with proper kernel implementations
void putchar(char c, uint32_t color);
void puts(char *str, uint32_t color, uint32_t length);
uint32_t move_to_child_directory(struct FAT32DriverRequest request);
uint32_t move_to_parent_directory(struct FAT32DriverRequest request);
void list_dir_content(char *content, uint32_t cluster);
void print(char *str, uint32_t color);
void search_dls_bm(char *text, uint32_t text_len, char *pattern);
void search_dls_kmp(char *text, uint32_t text_len, char *pattern);
void ps(char *buffer);
void print_path_to_dir(char *buffer, uint32_t cluster, char *path);

// Stub implementations for missing functions
void putchar(char c, uint32_t color) {
    framebuffer_put(c, (uint8_t)color);
}

void puts(char *str, uint32_t color, uint32_t length) {
    for (uint32_t i = 0; i < length && str[i] != '\0'; i++) {
        framebuffer_put(str[i], (uint8_t)color);
    }
}

uint32_t move_to_child_directory(struct FAT32DriverRequest request) {
    // Stub implementation - should be properly implemented
    return 0;
}

uint32_t move_to_parent_directory(struct FAT32DriverRequest request) {
    // Stub implementation - should be properly implemented
    return 0;
}

void list_dir_content(char *content, uint32_t cluster) {
    // Stub implementation - should be properly implemented
}

void print(char *str, uint32_t color) {
    for (int i = 0; str[i] != '\0'; i++) {
        framebuffer_put(str[i], (uint8_t)color);
    }
}

void search_dls_bm(char *text, uint32_t text_len, char *pattern) {
    // Stub implementation - should be properly implemented
}

void search_dls_kmp(char *text, uint32_t text_len, char *pattern) {
    // Stub implementation - should be properly implemented
}

void ps(char *buffer) {
    // Stub implementation - should be properly implemented
}

void print_path_to_dir(char *buffer, uint32_t cluster, char *path) {
    // Stub implementation - should be properly implemented
}

unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;

void activate_keyboard_interrupt(void) {
    out(PIC1_DATA, in(PIC1_DATA) & ~(1 << IRQ_KEYBOARD));
}

void io_wait(void) {
    out(0x80, 0);
}

void pic_ack(uint8_t irq) {
    if (irq >= 8) out(PIC2_COMMAND, PIC_ACK);
    out(PIC1_COMMAND, PIC_ACK);
}

void pic_remap(void) {
    out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); 
    io_wait();
    out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC1_DATA, PIC1_OFFSET); 
    io_wait();
    out(PIC2_DATA, PIC2_OFFSET); 
    io_wait();
    out(PIC1_DATA, 0b0100); 
    io_wait();
    out(PIC2_DATA, 0b0010); 
    io_wait();

    out(PIC1_DATA, ICW4_8086);
    io_wait();
    out(PIC2_DATA, ICW4_8086);
    io_wait();

    out(PIC1_DATA, PIC_DISABLE_ALL_MASK);
    out(PIC2_DATA, PIC_DISABLE_ALL_MASK);
}

void syscall(struct InterruptFrame frame)
{
  switch (frame.cpu.general.eax)
  {
  case 0:
    *((int8_t *)frame.cpu.general.ecx) = read(
        *(struct FAT32DriverRequest *)frame.cpu.general.ebx);
    break;
  case 1:
    *((int8_t *)frame.cpu.general.ecx) = read_directory(
        *(struct FAT32DriverRequest *)frame.cpu.general.ebx);
    break;
  case 2:
    *((int8_t *)frame.cpu.general.ecx) = write(
        *(struct FAT32DriverRequest *)frame.cpu.general.ebx);
    break;
  case 3:
    *((int8_t *)frame.cpu.general.ecx) = delete (
        *(struct FAT32DriverRequest *)frame.cpu.general.ebx);
    break;
  case 4:
    get_keyboard_buffer((char *)frame.cpu.general.ebx);
    break;
  case 5:
    putchar((char)frame.cpu.general.ebx, frame.cpu.general.ecx);
    break;
  case 6:
    puts(
        (char *)frame.cpu.general.ebx,
        frame.cpu.general.ecx,
        frame.cpu.general.edx); // Assuming puts() exist in kernel
    break;
  case 7:
    keyboard_state_activate();
    break;
  case (8):
    *((uint32_t *)frame.cpu.general.ecx) = move_to_child_directory(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
    break;
  case (9):
    *((uint32_t *)frame.cpu.general.ecx) = move_to_parent_directory(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
    break;
  case (10):
    list_dir_content((char *)frame.cpu.general.ebx, frame.cpu.general.ecx);
    break;
  case (11):
    print((char *)frame.cpu.general.ebx, frame.cpu.general.ecx);
    break;
  case (12):
    search_dls_bm((char *)frame.cpu.general.ebx, frame.cpu.general.ecx, (char *)frame.cpu.general.edx);
    break;
  case (19):
    search_dls_kmp((char *)frame.cpu.general.ebx, frame.cpu.general.ecx, (char *)frame.cpu.general.edx);
    break;
  case (13):
    framebuffer_clear();
    framebuffer_state.cur_col = 0;
    framebuffer_state.cur_row = 0;
    break;
  case (14):
    process_destroy(frame.cpu.general.ebx);
    break;
  case (15):
    process_create_user_process(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
    break;
  case (16):
    ps((char *)frame.cpu.general.ebx);
    break;
  case (17):
    read_rtc(&hour, &minute, &second);
    *((uint8_t *)frame.cpu.general.ebx) = hour;
    *((uint8_t *)frame.cpu.general.ecx) = minute;
    *((uint8_t *)frame.cpu.general.edx) = second;

    // Calculate the position to print the clock
    uint8_t clock_row = 24;     // The last row (25th row, zero-indexed)
    uint8_t clock_col = 80 - 8; // 80 columns wide minus 8 columns for "HH:MM:SS"

    // Clear the previous clock (if any)
    for (uint8_t i = 0; i < 8; i++)
    {
      framebuffer_write(clock_row - 1, clock_col + i, ' ', 0x07, 0x00);
    }

    // Print the new time
    framebuffer_write(clock_row, clock_col, (hour / 10) + '0', 0xA, 0x0);
    framebuffer_write(clock_row, clock_col + 1, (hour % 10) + '0', 0xA, 0x0);
    framebuffer_write(clock_row, clock_col + 2, ':', 0xA, 0x0);
    framebuffer_write(clock_row, clock_col + 3, (minute / 10) + '0', 0xA, 0x0);
    framebuffer_write(clock_row, clock_col + 4, (minute % 10) + '0', 0xA, 0x0);
    framebuffer_write(clock_row, clock_col + 5, ':', 0xA, 0x0);
    framebuffer_write(clock_row, clock_col + 6, (second / 10) + '0', 0xA, 0x0);
    framebuffer_write(clock_row, clock_col + 7, (second % 10) + '0', 0xA, 0x0);
    break;
  case (18):
    print_path_to_dir((char *)frame.cpu.general.ebx, frame.cpu.general.ecx, (char *)frame.cpu.general.edx);
    break;
  // case (18):
  //   *((int8_t *)frame.cpu.general.ecx) = move_dir(*(struct FAT32DriverRequest *)frame.cpu.general.ebx, *(struct FAT32DriverRequest *)frame.cpu.general.edx);
  //   break;
  }
}
void main_interrupt_handler(struct InterruptFrame frame) {
    switch (frame.int_number) {
        case IRQ_KEYBOARD + PIC1_OFFSET:
            keyboard_isr();
            break;
        case SYSCALL_CALL: 
            syscall(frame);
            break;
        case IRQ_TIMER + PIC1_OFFSET:
            struct Context ctx;
            ctx.cpu = frame.cpu;
            ctx.eflags = frame.int_stack.eflags;
            ctx.eip = frame.int_stack.eip;
            ctx.page_directory_virtual_addr = process_get_current_running_pcb_pointer()->context.page_directory_virtual_addr;
            scheduler_save_context_to_current_running_pcb(ctx);
            pic_ack(IRQ_TIMER);
            scheduler_switch_to_next_process();
            break;
        case 0xe: 
            uint32_t current_process = process_get_current_running_pcb_pointer()->metadata.pid;
            pic_ack(IRQ_TIMER); 
            scheduler_switch_to_next_process();
            process_destroy(current_process);
            break;
        case 0xd:    
            uint32_t current_process_to_kill = process_get_current_running_pcb_pointer()->metadata.pid;
            pic_ack(IRQ_TIMER); 
            scheduler_switch_to_next_process();
            process_destroy(current_process_to_kill);
            break;
    }
}

struct TSSEntry _interrupt_tss_entry = {
    .ss0  = GDT_KERNEL_DATA_SEGMENT_SELECTOR,
};

void set_tss_kernel_current_stack(void) {
    uint32_t stack_ptr;
    __asm__ volatile ("mov %%ebp, %0": "=r"(stack_ptr) : /* <Empty> */);
    _interrupt_tss_entry.esp0 = stack_ptr + 8; 
}
