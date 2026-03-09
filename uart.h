/* ─────────────────────────────────────────────────────────────────────────
 * uart.h + uart.c (combined for clarity)
 * UART Driver for LM3S6965 (ARM Cortex-M3)
 *
 * On a PC, printf() writes to your screen via the OS.
 * On a microcontroller, YOU write directly to UART hardware registers
 * to send bytes out over a serial line.
 *
 * This is REAL embedded C — direct memory-mapped register access.
 * No printf. No stdlib. Just raw hardware writes.
 *
 * LM3S6965 UART0 Register Map:
 *   Base address: 0x4000C000
 *   DR  (Data Register)   : offset 0x000 — write a byte here to transmit
 *   FR  (Flag Register)   : offset 0x018 — bit 5 = TX FIFO full
 * ───────────────────────────────────────────────────────────────────────── */

#ifndef UART_H
#define UART_H

#include <stdint.h>

/* ── UART0 Base Address (LM3S6965) ── */
#define UART0_BASE      0x4000C000UL

/* ── Register offsets ── */
#define UART_DR_OFFSET  0x000   /* Data Register    */
#define UART_FR_OFFSET  0x018   /* Flag Register    */

/* ── Flag Register bits ── */
#define UART_FR_TXFF    (1 << 5) /* TX FIFO Full — wait if this is set */

/* ── Macro to access any UART register ── */
#define UART_REG(base, offset) (*((volatile uint32_t *)((base) + (offset))))

/* ─── uart_putc() ────────────────────────────────────────────────────────
 * Send one character over UART.
 * Waits (busy-waits) until TX buffer has space, then writes the byte.
 * This is called "polling" — a real driver would use interrupts.
 * ─────────────────────────────────────────────────────────────────────── */
static inline void uart_putc(char c) {
    /* Wait until TX FIFO is not full */
    while (UART_REG(UART0_BASE, UART_FR_OFFSET) & UART_FR_TXFF);
    /* Write byte to Data Register — hardware transmits it */
    UART_REG(UART0_BASE, UART_DR_OFFSET) = (uint32_t)c;
}

/* ─── uart_puts() ────────────────────────────────────────────────────────
 * Send a null-terminated string over UART.
 * ─────────────────────────────────────────────────────────────────────── */
static inline void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

/* ─── uart_put_int() ─────────────────────────────────────────────────────
 * Send an integer over UART (no printf, manual conversion).
 * ─────────────────────────────────────────────────────────────────────── */
static inline void uart_put_int(int n) {
    if (n < 0) { uart_putc('-'); n = -n; }
    if (n == 0) { uart_putc('0'); return; }
    char buf[12];
    int i = 0;
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    /* Print digits in reverse */
    while (i > 0) uart_putc(buf[--i]);
}

/* ─── uart_put_float() ───────────────────────────────────────────────────
 * Send a float with 1 decimal place over UART.
 * (No floating point library — manual integer math)
 * ─────────────────────────────────────────────────────────────────────── */
static inline void uart_put_float(int whole, int frac) {
    uart_put_int(whole);
    uart_putc('.');
    uart_put_int(frac);
}

#endif /* UART_H */
