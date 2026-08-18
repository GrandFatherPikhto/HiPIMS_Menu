#ifndef HIPIMS_SPI_H
#define HIPIMS_SPI_H

#include <stdbool.h>
#include <stdint.h>

/* Register map v2 — spi_regs_pkg.vhd on the FPGA side. See
 * techdocs/notes/new_prompt.md and techdocs/notes/handoff.md. */
#define HIPIMS_REG_ENABLE      0
#define HIPIMS_REG_START       1
#define HIPIMS_REG_PERIOD      2
#define HIPIMS_REG_LEGA_WIDTH  3
#define HIPIMS_REG_LEGB_ENABLE 4
#define HIPIMS_REG_LEGB_DELAY  5
#define HIPIMS_REG_LEGB_WIDTH  6
#define HIPIMS_REG_CH_BASE     7

#define HIPIMS_NUM_PROBE_CH    8

/* Anode channel (SiC half-bridge with fault/reset handling), added 2026-08-18. */
#define HIPIMS_REG_ANODE_BASE     (HIPIMS_REG_CH_BASE + HIPIMS_NUM_PROBE_CH * 3) /* 31 */
#define HIPIMS_REG_ANODE_ENABLE   (HIPIMS_REG_ANODE_BASE + 0)
#define HIPIMS_REG_ANODE_PERIOD   (HIPIMS_REG_ANODE_BASE + 1)
#define HIPIMS_REG_ANODE_DELAY    (HIPIMS_REG_ANODE_BASE + 2)
#define HIPIMS_REG_ANODE_DURATION (HIPIMS_REG_ANODE_BASE + 3)
#define HIPIMS_REG_ANODE_DEADTIME (HIPIMS_REG_ANODE_BASE + 4) /* addition on top of the 300ns hardwired floor, raw cycles, unsigned */

#define HIPIMS_NUM_REGS (HIPIMS_REG_ANODE_BASE + 5) /* 36 */

#define HIPIMS_REG_CH_ENABLE(ch) (HIPIMS_REG_CH_BASE + (ch) * 3)
#define HIPIMS_REG_CH_DELAY(ch)  (HIPIMS_REG_CH_BASE + (ch) * 3 + 1)
#define HIPIMS_REG_CH_WIDTH(ch)  (HIPIMS_REG_CH_BASE + (ch) * 3 + 2)

/* pulse_gen_pkg.vhd limits — see techdocs/notes/handoff.md. */
#define HIPIMS_TIME_WIDTH_MAX    8388607 /* 2^23-1 raw cycles: period/width field width */
#define HIPIMS_MAX_PERIOD_CYCLES 8338607 /* safe max for REG_PERIOD (2^23-1 minus pre/post margin) */
#define HIPIMS_MARGIN_CYCLES     25000   /* 500us pre/post margin around the pulse window */

/* Status byte read back over MISO on byte 6 of every frame. */
#define HIPIMS_STATUS_ACK     (1u << 0)
#define HIPIMS_STATUS_FAULT_A (1u << 1) /* live, not tied to the triggering transaction */
#define HIPIMS_STATUS_FAULT_B (1u << 2)

void hipims_spi_init(void);

/* One CS_N low -> 7-byte frame (addr + 32-bit signed big-endian value +
 * CRC-8) -> dummy byte while reading the status byte back over MISO -> CS_N
 * high. Retries the whole frame up to HIPIMS_SPI_MAX_RETRIES times on NACK.
 * Returns false if the FPGA never ACKed — the register was not written. */
bool hipims_spi_write_reg(uint8_t reg_addr, int32_t value);

/* Status byte from the last completed transaction (ACK/NACK + live fault bits). */
uint8_t hipims_spi_last_status(void);

#endif /* HIPIMS_SPI_H */
