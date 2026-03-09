/* ─────────────────────────────────────────────────────────────────────────
 * patient.h
 * Patient struct and vital sign thresholds
 *
 * Vitals stored as integers (x10) to avoid floating point library.
 * e.g. 36.5C is stored as 365, 98.6 bpm stored as 986
 * This is standard embedded practice — no FPU needed.
 * ───────────────────────────────────────────────────────────────────────── */

#ifndef PATIENT_H
#define PATIENT_H

#include <stdint.h>

/* ── Thresholds (multiplied by 10 to avoid floats) ── */
#define HR_MIN        600    /* 60.0 bpm  */
#define HR_MAX       1000    /* 100.0 bpm */
#define HR_WARN_LOW   500    /* 50.0 bpm  */
#define HR_WARN_HIGH 1200    /* 120.0 bpm */

#define BP_MIN        900    /* 90.0 mmHg */
#define BP_MAX       1200    /* 120.0 mmHg */
#define BP_WARN_HIGH 1500    /* 150.0 mmHg */

#define TEMP_MIN      361    /* 36.1 C */
#define TEMP_MAX      372    /* 37.2 C */
#define TEMP_WARN_H   390    /* 39.0 C */
#define TEMP_WARN_L   350    /* 35.0 C */

/* ── Alert Levels ── */
#define ALERT_STABLE   0
#define ALERT_WATCH    1
#define ALERT_CRITICAL 2

/* ── Patient Struct ── */
typedef struct {
    uint8_t  id;
    char     name[24];
    uint8_t  bed;
    int      heart_rate;      /* stored x10, e.g. 756 = 75.6 bpm  */
    int      blood_pressure;  /* stored x10, e.g. 1185 = 118.5 mmHg */
    int      temperature;     /* stored x10, e.g. 368 = 36.8 C     */
    uint8_t  alert_level;
} Patient;

#endif /* PATIENT_H */
