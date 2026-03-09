# ─────────────────────────────────────────────────────────────────────────
# Makefile — ICU Monitor Bare-Metal ARM Cortex-M3
#
# Usage (in MSYS2 terminal):
#   mingw32-make        → compile only
#   mingw32-make run    → compile + run on QEMU
#   mingw32-make clean  → remove build files
# ─────────────────────────────────────────────────────────────────────────

# ── Toolchain ──
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# ── Target MCU: ARM Cortex-M3 ──
CPU     = -mcpu=cortex-m3 -mthumb

# ── Compiler Flags ──
#   -nostdlib       : no standard C library (bare metal!)
#   -nostartfiles   : we provide our own startup.c
#   -ffreestanding  : no OS assumptions
#   -Wall           : show all warnings
CFLAGS  = $(CPU) -nostdlib -nostartfiles -ffreestanding -Wall -O0 -g

# ── Source Files ──
SRCS    = startup.c main.c

# ── Output Files ──
ELF     = icu_monitor.elf   # compiled binary (ELF format)
BIN     = icu_monitor.bin   # raw binary (for flashing real hardware)

# ── QEMU Settings ──
# -M lm3s6965evb  : emulate the LM3S6965 board (has UART0)
# -nographic      : output UART to terminal (no GUI window)
# -kernel         : load our ELF binary
QEMU    = "C:/Program Files/qemu/qemu-system-arm.exe"
QFLAGS  = -M lm3s6965evb -nographic -kernel $(ELF)

# ────────────────────────────────────────────────────────────────────────
# Build Rules
# ────────────────────────────────────────────────────────────────────────

all: $(ELF)

# Link everything into an ELF binary
$(ELF): $(SRCS) linker.ld
	$(CC) $(CFLAGS) -T linker.ld $(SRCS) -o $(ELF)
	@echo ""
	@echo "  Build successful: $(ELF)"
	@echo "  Run with: mingw32-make run"
	@echo ""

# Run on QEMU (press Ctrl+A then X to exit)
run: $(ELF)
	@echo ""
	@echo "  Starting QEMU — ARM Cortex-M3 emulator..."
	@echo "  Press Ctrl+A then X to exit QEMU"
	@echo ""
	$(QEMU) $(QFLAGS)

# Clean build artifacts
clean:
	del /f $(ELF) $(BIN) 2>nul || true
	@echo "  Cleaned."
