/* ─────────────────────────────────────────────────────────────────────────
 * linker.ld
 * Linker script for ARM Cortex-M3 (LM3S6965 — emulated by QEMU)
 *
 * Tells the linker WHERE to place code and data in memory.
 * This is a fundamental embedded concept — on a PC the OS handles this,
 * on a microcontroller YOU define the memory map.
 *
 * LM3S6965 Memory Map:
 *   Flash (ROM): starts at 0x00000000, size 256KB  ← code lives here
 *   SRAM:        starts at 0x20000000, size 64KB   ← variables live here
 * ───────────────────────────────────────────────────────────────────────── */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    /* ── .text: all code goes into FLASH ── */
    .text :
    {
        KEEP(*(.vector_table))   /* Vector table MUST be first */
        *(.text*)                /* All code sections          */
        *(.rodata*)              /* Read-only data (constants) */
    } > FLASH

    /* ── .data: initialized variables — stored in FLASH, copied to SRAM ── */
    _data_start_flash = LOADADDR(.data);
    .data :
    {
        _data_start = .;
        *(.data*)
        _data_end = .;
    } > SRAM AT > FLASH

    /* ── .bss: zero-initialized variables — live in SRAM ── */
    .bss :
    {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    } > SRAM

    /* ── Stack: grows downward from top of SRAM ── */
    _stack_top = ORIGIN(SRAM) + LENGTH(SRAM);
}
