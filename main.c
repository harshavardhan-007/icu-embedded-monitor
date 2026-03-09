/* ─────────────────────────────────────────────────────────────────────────
 * main.c
 * ICU Patient Vital Signs Monitor — Bare-Metal Embedded C
 * Target: ARM Cortex-M3 (LM3S6965, emulated by QEMU)
 *
 * NO standard library. NO printf. NO OS.
 * Output via direct UART register writes.
 * Compiled with arm-none-eabi-gcc → real ARM machine code.
 * ───────────────────────────────────────────────────────────────────────── */

#include <stdint.h>
#include "uart.h"
#include "patient.h"

/* ─── String copy (no stdlib strcpy) ────────────────────────────────────
 * Simple character-by-character copy. Avoids memcpy/strcpy dependency.
 * ─────────────────────────────────────────────────────────────────────── */
static void uart_puts_copy(char *dst, const char *src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

/* ─── Simple pseudo-random number generator ─────────────────────────────
 * No stdlib rand() available — implement our own using LCG algorithm.
 * This is common in bare-metal embedded C.
 * ─────────────────────────────────────────────────────────────────────── */
static uint32_t seed = 12345;

static uint32_t rand_next(void) {
    seed = seed * 1664525UL + 1013904223UL;  /* LCG formula */
    return seed;
}

/* Returns a pseudo-random int between min and max (x10 scale) */
static int rand_range(int min, int max) {
    uint32_t r = rand_next();
    return min + (int)(r % (uint32_t)(max - min + 1));
}

/* ─── simulate_vitals() ──────────────────────────────────────────────────
 * Fills patient vitals with simulated sensor readings.
 * 30% chance each vital is abnormal — keeps demo realistic.
 * ─────────────────────────────────────────────────────────────────────── */
static void simulate_vitals(Patient *p) {
    int scenario = (int)(rand_next() % 10);

    /* Heart Rate */
    if      (scenario < 2) p->heart_rate = rand_range(HR_WARN_LOW, HR_MIN - 1);
    else if (scenario < 4) p->heart_rate = rand_range(HR_MAX + 1, HR_WARN_HIGH);
    else                   p->heart_rate = rand_range(HR_MIN, HR_MAX);

    /* Blood Pressure */
    if (scenario < 3) p->blood_pressure = rand_range(BP_MAX + 1, BP_WARN_HIGH);
    else              p->blood_pressure = rand_range(BP_MIN, BP_MAX);

    /* Temperature */
    if      (scenario < 2) p->temperature = rand_range(TEMP_WARN_H, 405);
    else if (scenario == 2) p->temperature = rand_range(340, TEMP_WARN_L);
    else                    p->temperature = rand_range(TEMP_MIN, TEMP_MAX);
}

/* ─── evaluate_alert() ───────────────────────────────────────────────────
 * Counts abnormal vitals and sets alert level.
 * ─────────────────────────────────────────────────────────────────────── */
static void evaluate_alert(Patient *p) {
    int abnormal = 0;
    if (p->heart_rate < HR_MIN     || p->heart_rate > HR_MAX)     abnormal++;
    if (p->blood_pressure < BP_MIN || p->blood_pressure > BP_MAX) abnormal++;
    if (p->temperature < TEMP_MIN  || p->temperature > TEMP_MAX)  abnormal++;

    if      (abnormal == 0) p->alert_level = ALERT_STABLE;
    else if (abnormal == 1) p->alert_level = ALERT_WATCH;
    else                    p->alert_level = ALERT_CRITICAL;
}

/* ─── print_vital() ──────────────────────────────────────────────────────
 * Prints one vital sign via UART (no printf).
 * Value is stored x10, so 756 prints as "75.6"
 * ─────────────────────────────────────────────────────────────────────── */
static void print_vital(const char *label, int value,
                         const char *unit, int is_abnormal) {
    uart_puts("    ");
    uart_puts(label);
    uart_puts(": ");
    uart_put_float(value / 10, value % 10);
    uart_putc(' ');
    uart_puts(unit);
    uart_puts("  ");
    uart_puts(is_abnormal ? "[!! ABNORMAL]" : "[   normal  ]");
    uart_puts("\r\n");
}

/* ─── print_patient() ────────────────────────────────────────────────────
 * Prints full patient reading to UART terminal.
 * ─────────────────────────────────────────────────────────────────────── */
static void print_patient(const Patient *p) {
    uart_puts("\r\n  +--------------------------------------------+\r\n");
    uart_puts("  | Bed ");
    uart_put_int(p->bed);
    uart_puts("  |  ");
    uart_puts(p->name);
    uart_puts("\r\n  +--------------------------------------------+\r\n");

    print_vital("Heart Rate (bpm)   ", p->heart_rate,
                "bpm ", (p->heart_rate < HR_MIN || p->heart_rate > HR_MAX));
    print_vital("Blood Pressure     ", p->blood_pressure,
                "mmHg", (p->blood_pressure < BP_MIN || p->blood_pressure > BP_MAX));
    print_vital("Temperature        ", p->temperature,
                "C   ", (p->temperature < TEMP_MIN || p->temperature > TEMP_MAX));

    uart_puts("  >> STATUS: ");
    switch (p->alert_level) {
        case ALERT_STABLE:
            uart_puts("[ STABLE   ] All vitals normal\r\n"); break;
        case ALERT_WATCH:
            uart_puts("[ WATCH    ] One vital out of range\r\n"); break;
        case ALERT_CRITICAL:
            uart_puts("[!CRITICAL!] *** NURSE ALERT TRIGGERED ***\r\n"); break;
    }
}

/* ─── main() ─────────────────────────────────────────────────────────────
 * Entry point — no OS, no setup needed beyond what startup.c did.
 * ─────────────────────────────────────────────────────────────────────── */
int main(void) {
    /* Define 5 ICU patients — initialized one by one to avoid memcpy */
    Patient patients[5];

    patients[0].id = 1; patients[0].bed = 3;
    uart_puts_copy(patients[0].name, "Rajesh Kumar", 24);

    patients[1].id = 2; patients[1].bed = 7;
    uart_puts_copy(patients[1].name, "Meena Devi", 24);

    patients[2].id = 3; patients[2].bed = 12;
    uart_puts_copy(patients[2].name, "Arjun Sharma", 24);

    patients[3].id = 4; patients[3].bed = 2;
    uart_puts_copy(patients[3].name, "Lakshmi Pillai", 24);

    patients[4].id = 5; patients[4].bed = 9;
    uart_puts_copy(patients[4].name, "Mohammed Rafi", 24);

    /* Initialize vitals to zero */
    for (int i = 0; i < 5; i++) {
        patients[i].heart_rate     = 0;
        patients[i].blood_pressure = 0;
        patients[i].temperature    = 0;
        patients[i].alert_level    = ALERT_STABLE;
    }

    int critical_total = 0;

    /* Header */
    uart_puts("\r\n================================================\r\n");
    uart_puts("||   ICU PATIENT VITAL SIGNS MONITOR v2.0   ||\r\n");
    uart_puts("||   Bare-Metal ARM Cortex-M3 | QEMU        ||\r\n");
    uart_puts("||   No OS | No stdlib | Direct UART Regs   ||\r\n");
    uart_puts("================================================\r\n");

    /* Main monitoring loop — 5 cycles */
    for (int cycle = 1; cycle <= 5; cycle++) {
        uart_puts("\r\n======== CYCLE ");
        uart_put_int(cycle);
        uart_puts(" / 5 =============================\r\n");

        for (int i = 0; i < 5; i++) {
            simulate_vitals(&patients[i]);
            evaluate_alert(&patients[i]);
            print_patient(&patients[i]);
            if (patients[i].alert_level == ALERT_CRITICAL)
                critical_total++;
        }
    }

    /* Summary */
    uart_puts("\r\n================================================\r\n");
    uart_puts("||            SESSION SUMMARY                ||\r\n");
    uart_puts("================================================\r\n");
    uart_puts("  Cycles run       : 5\r\n");
    uart_puts("  Patients watched : 5\r\n");
    uart_puts("  CRITICAL alerts  : ");
    uart_put_int(critical_total);
    uart_puts("\r\n================================================\r\n");

    /* On real hardware, never return from main */
    while(1);
    return 0;
}
