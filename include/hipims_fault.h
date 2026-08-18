#ifndef HIPIMS_FAULT_H
#define HIPIMS_FAULT_H

#include <stdbool.h>

void hipims_fault_init(void);

/* Call at a steady rate (e.g. every 10ms) from the main loop — polls the
 * status byte over SPI and drives the fault LED. */
void hipims_fault_tick(void);

bool hipims_fault_a(void);
bool hipims_fault_b(void);

/* The fault latch on the FPGA clears only on a REG_ENABLE 0->1 edge (by
 * design, 2026-08-18 — the old self-clearing behaviour raced against how
 * slowly STM32 finds out about a fault over SPI, letting the generator
 * restart before the firmware even knew). Writes ENABLE=0, holds briefly,
 * then ENABLE=1. If the fault is still physically present it will
 * immediately re-latch — that is expected, not a bug. Returns false only if
 * either SPI transaction itself failed (a comms error, not a re-fault). */
bool hipims_fault_reset(void);

#endif /* HIPIMS_FAULT_H */
