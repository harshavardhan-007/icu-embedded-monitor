/* ─────────────────────────────────────────────────────────────────────────
 * startup.c
 * Bare-metal startup code for ARM Cortex-M3
 *
 * On a PC, the OS sets up your program before main() runs.
 * On a microcontroller, YOU write this startup code yourself.
 *
 * This file does 3 things:
 *   1. Defines the Vector Table (tells CPU where to jump on reset/errors)
 *   2. Copies .data section from FLASH to SRAM
 *   3. Zeroes out .bss section
 *   4. Calls main()
 * ───────────────────────────────────────────────────────────────────────── */

#include <stdint.h>

/* ── Symbols defined in linker.ld ── */
extern uint32_t _data_start;
extern uint32_t _data_end;
extern uint32_t _data_start_flash;
extern uint32_t _bss_start;
extern uint32_t _bss_end;
extern uint32_t _stack_top;

/* ── Forward declaration of main ── */
extern int main(void);

/* ─── Default fault handlers ────────────────────────────────────────────── */
void Default_Handler(void) { while(1); }  /* Infinite loop on fault */
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));

/* ─── Reset_Handler ─────────────────────────────────────────────────────
 * This is the FIRST function that runs when the MCU powers on or resets.
 * ─────────────────────────────────────────────────────────────────────── */
void Reset_Handler(void) {
    /* Step 1: Copy .data from FLASH to SRAM */
    uint32_t *src = &_data_start_flash;
    uint32_t *dst = &_data_start;
    while (dst < &_data_end) {
        *dst++ = *src++;
    }

    /* Step 2: Zero out .bss section in SRAM */
    dst = &_bss_start;
    while (dst < &_bss_end) {
        *dst++ = 0;
    }

    /* Step 3: Call main */
    main();

    /* Step 4: If main returns (it shouldn't), loop forever */
    while(1);
}

/* ─── Vector Table ──────────────────────────────────────────────────────
 * The ARM Cortex-M3 reads this table on startup.
 * Entry 0 = initial stack pointer
 * Entry 1 = address of Reset_Handler (first code to run)
 * ─────────────────────────────────────────────────────────────────────── */
__attribute__((section(".vector_table")))
uint32_t vector_table[] = {
    (uint32_t)&_stack_top,       /* 0: Initial Stack Pointer */
    (uint32_t)&Reset_Handler,    /* 1: Reset Handler         */
    (uint32_t)&NMI_Handler,      /* 2: NMI Handler           */
    (uint32_t)&HardFault_Handler,/* 3: Hard Fault Handler    */
};
