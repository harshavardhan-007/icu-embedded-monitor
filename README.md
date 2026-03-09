Here it is — select all and copy:

---

# 🏥 ICU Patient Vital Signs Monitor
### Bare-Metal Embedded C | ARM Cortex-M3 | QEMU

---

## 🔴 The Real Problem
In Indian government hospitals, the nurse-to-patient ratio is often **1:20**. Nurses physically cannot check every patient frequently enough. Patients in critical condition can deteriorate unnoticed.

This project is a **bare-metal embedded C implementation** of an early warning system that continuously monitors ICU patient vitals and escalates alerts before a patient's condition becomes life-threatening — the kind of system built by companies like **Philips Healthcare, GE Healthcare, and Siemens Healthineers**.

---

## ⚙️ Technical Highlights

| Feature | Details |
|---|---|
| **Target MCU** | ARM Cortex-M3 (LM3S6965) |
| **Emulator** | QEMU `lm3s6965evb` |
| **Compiler** | `arm-none-eabi-gcc` |
| **Standard Library** | ❌ None (`-nostdlib -nostartfiles`) |
| **OS** | ❌ None (bare-metal) |
| **Output** | Direct UART register writes (no printf) |
| **Boot** | Custom vector table + startup code |
| **Memory Map** | Custom linker script (FLASH @ 0x00000000, SRAM @ 0x20000000) |

---

## 📁 Project Structure

```
icu-embedded-monitor/
├── main.c       → Main monitoring loop, patient logic, UART output
├── startup.c    → Vector table, Reset_Handler, RAM initialization
├── uart.h       → Direct UART0 register driver (no printf)
├── patient.h    → Patient struct, vital thresholds, alert levels
├── linker.ld    → Custom memory map (FLASH + SRAM layout)
└── Makefile     → Build and run commands
```

---

## 🏥 What It Does

Monitors **3 vitals** per patient across **5 ICU patients**:

| Vital | Normal Range |
|---|---|
| 🫀 Heart Rate | 60 – 100 bpm |
| 🩸 Blood Pressure | 90 – 120 mmHg |
| 🌡️ Body Temperature | 36.1 – 37.2 °C |

### 3-Tier Alert System:

| Level | Condition | Action |
|---|---|---|
| `STABLE` | All vitals normal | No action |
| `WATCH` | One vital out of range | Monitor closely |
| `CRITICAL` | Two or more vitals abnormal | Nurse alert triggered |

---

## 🚀 How to Build & Run

### Requirements
- [MSYS2](https://www.msys2.org) with `mingw-w64-ucrt-x86_64-arm-none-eabi-gcc`
- [QEMU](https://www.qemu.org/download/#windows) with `qemu-system-arm`

### Compile
```bash
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -nostdlib -nostartfiles \
  -ffreestanding -fno-builtin -Wall \
  -T linker.ld startup.c main.c -o icu_monitor.elf
```

### Run on QEMU
```bash
"C:/Program Files/qemu/qemu-system-arm.exe" -M lm3s6965evb -nographic -kernel icu_monitor.elf
```

> Press `Ctrl+A` then `X` to exit QEMU

---

## 📺 Sample Output

```
================================================
||   ICU PATIENT VITAL SIGNS MONITOR v2.0   ||
||   Bare-Metal ARM Cortex-M3 | QEMU        ||
||   No OS | No stdlib | Direct UART Regs   ||
================================================

  +--------------------------------------------+
  | Bed 7  |  Meena Devi
  +--------------------------------------------+
    Heart Rate (bpm)   : 59.1 bpm    [!! ABNORMAL]
    Blood Pressure     : 127.9 mmHg  [!! ABNORMAL]
    Temperature        : 39.1 C      [!! ABNORMAL]
  >> STATUS: [!CRITICAL!] *** NURSE ALERT TRIGGERED ***
```

---

## 🧠 Key Embedded C Concepts Demonstrated

- **Vector Table** — defined in `startup.c`, placed at address `0x00000000`
- **Reset Handler** — copies `.data` from FLASH to SRAM, zeroes `.bss`
- **Memory-mapped I/O** — UART output via direct register access (`0x4000C000`)
- **Linker Script** — custom FLASH/SRAM memory layout for ARM Cortex-M3
- **No stdlib** — integer math, string handling, RNG all implemented manually
- **Struct-based design** — `Patient` struct models real sensor data

---

## 📝 Resume Line

> *"Developed a bare-metal ICU patient vital signs monitor in Embedded C targeting ARM Cortex-M3 (LM3S6965), compiled with arm-none-eabi-gcc. Implemented custom startup code, vector table, linker script, and direct UART register communication with zero standard library dependency. Emulated on QEMU. Inspired by nurse workload challenges in Indian public hospitals."*

---

## 👨‍💻 Author
**Harshavardhan KS**
