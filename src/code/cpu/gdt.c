#include "../../header/cpu/gdt.h"
#include "../../header/cpu/interrupt.h"

struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        // Null Descriptor
        {
            // TODO : Implement
            .segment_low = 0,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0,
            .non_system = 0,
            .privilege_level = 0,
            .present = 0,
            .segment_high = 0,
            .available = 0,
            .l_bit = 0,
            .default_operation_size = 0,
            .granularity = 0,
            .base_high = 0},
        // Kernel Code
        {
            // TODO : Implement
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0xA,
            .non_system = 1,
            .privilege_level = 0,
            .present = 1,
            .segment_high = 0xF,
            .available = 0,
            .l_bit = 0,
            .default_operation_size = 1,
            .granularity = 1,
            .base_high = 0},
        // Kernel Data
        {
            // TODO : Implement
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0x2,
            .non_system = 1,
            .privilege_level = 0,
            .present = 1,
            .segment_high = 0xF,
            .available = 0,
            .l_bit = 0,
            .default_operation_size = 1,
            .granularity = 1,
            .base_high = 0},
        //    User Code
        {
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0xA,
            .non_system = 1,
            .privilege_level = 0x3,
            .present = 1,
            .segment_high = 0xF,
            .available = 0,
            .l_bit = 0,
            .default_operation_size = 1,
            .granularity = 1,
            .base_high = 0},
        //  User Data
        {
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0x2,
            .non_system = 1,
            .privilege_level = 0x3,
            .present = 1,
            .segment_high = 0xF,
            .available = 0,
            .l_bit = 0,
            .default_operation_size = 1,
            .granularity = 1,
            .base_high = 0},
        {
            .segment_high = (sizeof(struct TSSEntry) & (0xF << 16)) >> 16,
            .segment_low = sizeof(struct TSSEntry),
            .base_high = 0,
            .base_mid = 0,
            .base_low = 0,
            .non_system = 0, // S bit
            .type_bit = 0x9,
            .privilege_level = 0,        // DPL
            .present = 1,                // P bit
            .default_operation_size = 1, // D/B bit
            .l_bit = 0,                  // L bit
            .granularity = 0,            // G bit
        },
        {0}}};

struct GDTR _gdt_gdtr = {
    .size = sizeof(global_descriptor_table),
    .address = &global_descriptor_table
};

void gdt_install_tss(void)
{
    uint32_t base = (uint32_t)&_interrupt_tss_entry;
    global_descriptor_table.table[5].base_high = (base & (0xFF << 24)) >> 24;
    global_descriptor_table.table[5].base_mid = (base & (0xFF << 16)) >> 16;
    global_descriptor_table.table[5].base_low = base & 0xFFFF;
}
