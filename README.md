# HiPIMS_Menu

STM32F103C8T6 ("Blue Pill") firmware providing the user interface (LCD1602
menu, EC11 rotary encoder) for a HIPIMS pulse generator whose timing-critical
core runs on a separate FPGA board. This firmware is the SPI master; the FPGA
is the SPI slave and the only thing that actually generates pulses.

The FPGA design lives in a separate repository, `HIPIMS_Generator`
(Quartus/VHDL project, EP4CE6E22C8 / Cyclone IV E). This repository does not
contain and does not track any VHDL — it only implements the protocol the
FPGA expects.

Русская версия: [README_ru.md](README_ru.md).

## Why the UI lives on an STM32 and not on the FPGA

The menu, LCD driver and encoder handling were originally implemented
directly in VHDL on the FPGA. That version worked and was timing-clean, but
consumed 54% of the device's logic elements (3363/6272 LE) and was too
complex for the project owner to maintain and extend on his own. Moving the
UI off the FPGA and onto a cheap STM32 as an SPI master dropped FPGA usage to
~32% (1996 LE) and improved setup timing slack from ~1ns to 11.4ns. The FPGA
is now "dumb": it only holds the timing-critical pulse generation core and a
SPI register file. This decision is final — it should not be reversed.

## Hardware

- **STM32F103C8T6 "Blue Pill"** — bare chip, no Wi-Fi. 72MHz (HSE 8MHz ×
  PLL9), APB1=36MHz, APB2=72MHz.
- **LCD1602** via a **PCF8574** I2C backpack (address `0x27`), 5V-powered.
- **EC11 rotary encoder + push button** for menu navigation and value
  editing.
- **FPGA link**: SPI, STM32 as master, EP4CE6E22C8 as slave.
- **Fault LED**: lit whenever the anode channel reports a live fault.
- **Debug UART** (USART1): TX-only diagnostic console, added for hardware
  bring-up. See [Debug UART](#debug-uart-temporary) below — it is explicitly
  provisional, not a permanent feature.
- ST7789/240x320 display is a deferred option, not implemented.

## Firmware stack

- PlatformIO, `board = bluepill_f103c8`, `framework = stm32cube`.
- **LL (Low-Layer) ST drivers only** — no HAL, no Arduino/STM32duino. This
  was an explicit, non-negotiable choice by the project owner: direct
  register-level access via named LL functions/macros, no hidden HAL
  abstraction magic.
- `build_flags = -DUSE_FULL_LL_DRIVER` is required to get the higher-level LL
  `*_InitTypeDef` structs (`LL_SPI_InitTypeDef`, `LL_I2C_InitTypeDef`,
  `LL_TIM_ENCODER_InitTypeDef`, `LL_USART_InitTypeDef`) — without it only the
  inline register-bit helpers are available.
- Flashing/debugging via a CMSIS-DAP probe: `upload_protocol = cmsis-dap`,
  `debug_tool = cmsis-dap` (the board's default is stlink, which this probe
  is not).

Code style throughout the project: simple, flat, readable C. No premature
abstractions, no framework built for a menu state machine — that complexity
is exactly what made the old VHDL UI unmaintainable, and the whole point of
the rewrite was to avoid repeating that mistake.

## Pinout

Confirmed with the project owner 2026-08-18. No AFIO remap needed anywhere —
everything uses default peripheral pin mapping.

### SPI1 → FPGA (`HIPIMS_Generator`)

| Pin | Function |
|-----|----------|
| PA4 | CS_N — software GPIO (not hardware NSS; held low across a multi-byte transaction) |
| PA5 | SCK |
| PA6 | MISO — FPGA drives this with the status/ACK byte (protocol v2) |
| PA7 | MOSI |

### I2C1 → LCD1602 (via PCF8574)

| Pin | Function |
|-----|----------|
| PB6 | SCL |
| PB7 | SDA |

LCD1602 + PCF8574 run on 5V; PB6/PB7 are 5V-tolerant (FT) pins configured
open-drain, pull-ups already present on the PCF8574 board.

### EC11 encoder

| Pin | Function |
|-----|----------|
| PA0 | TIM2_CH1 = encoder channel A |
| PA1 | TIM2_CH2 = encoder channel B |
| PA2 | encoder push button (GPIO, pull-up, button to GND) |

A/B are wired to TIM2 specifically to use its hardware **Encoder Mode**
(`LL_TIM_ENCODERMODE_X4_TI12`) — the timer counts quadrature edges in
hardware (`CNT` register), no per-edge interrupt needed. Firmware just polls
the delta periodically.

### Fault indication

| Pin | Function |
|-----|----------|
| PB0 | output, driven high whenever `fault_a_live` or `fault_b_live` is set |

### Debug UART (USART1) — temporary

| Pin  | Function |
|------|----------|
| PA9  | TX, 115200 8N1 |
| PA10 | RX — wired but unused by firmware (TX-only diagnostics) |

### Free / reserved

PA3, PB1, PB3–5, PB8–15, PC13–15 are free for future use (e.g. ST7789 — do
not start that without an explicit request). PA13/PA14 are SWD and must not
be repurposed.

## SPI protocol v2

Full duplex, Mode 0 (CPOL=0, CPHA=0), MSB first, `CS_N` driven manually by
firmware and held low across multi-frame bursts.

A unit of transfer is **7 bytes**:

| Byte | Content |
|------|---------|
| 0 | register address, `0..NUM_REGS-1` |
| 1–4 | 32-bit signed value, big-endian |
| 5 | CRC-8 over bytes 0–4 |
| 6 | master sends a dummy byte, and simultaneously reads back a status byte over MISO |

CRC-8: polynomial `x^8+x^2+x^1+1` (`0x07`), init `0x00`, no input/output
reflection, no final XOR — the SMBus PEC variant. Implemented byte-at-a-time
in `hipims_crc8()` (`src/hipims_spi.c`), verified numerically identical to
the FPGA's bit-serial VHDL implementation.

Status byte (read back on byte 6):

| Bit | Meaning |
|-----|---------|
| 0 | ACK — 1 if the CRC matched and (if the address was in range) the write committed |
| 1 | `fault_a` live — not tied to this specific transaction |
| 2 | `fault_b` live |
| 3–7 | reserved, 0 |

### Retries and the FPGA glitch filter

The FPGA's SPI slave has a glitch filter on `SCK`/`MOSI`/`CS_N` that rejects
any transition shorter than 4 FPGA clocks (80ns @ 50MHz) — added after real
power-switching noise from the anode channel was observed corrupting SPI
transactions. This constrains how retries must be structured:

1. **Never resend a 7-byte unit under the same `CS_N low` session.** If a
   glitch injected a spurious edge into `SCK`, the FPGA's bit/byte counter is
   desynced, and it only resets on `CS_N` rising. Resending in the same
   session just produces NACK after NACK. Correct: `CS_N` high → gap →
   `CS_N` low → resend all 7 bytes from scratch.
2. **The `CS_N`-high gap between retries must be held well above 80ns.**
   Two back-to-back GPIO register writes at 72MHz can land in ~30–60ns,
   which the glitch filter would reject as noise, making the retry itself
   invisible to the FPGA. `hipims_spi_cs_gap_delay()` inserts an explicit
   ~200-iteration busy loop between attempts.

`hipims_spi_write_reg()` retries up to `HIPIMS_SPI_MAX_RETRIES` (10) times on
NACK before giving up and returning `false`. The FPGA never retries anything
on its own.

## Register map

`spi_regs_pkg.vhd` on the FPGA side, 0-based addresses, `NUM_REGS = 36`. All
time/width/delay/period registers are **raw 50MHz FPGA cycles** (1 LSB =
20ns), never microseconds or milliseconds — conversion to human units happens
entirely on the STM32 side (`raw_cycles = us * 50`).

```
REG_ENABLE      = 0   -- 0/1, global generation enable
REG_START       = 1   -- 0/1, physical "permission" pin, independent of ENABLE
REG_PERIOD      = 2   -- period, raw cycles
REG_LEGA_WIDTH  = 3   -- leg A pulse width, raw cycles
REG_LEGB_ENABLE = 4   -- 0/1, leg B (not physically populated yet)
REG_LEGB_DELAY  = 5   -- signed delay of leg B relative to leg A's start, raw cycles
REG_LEGB_WIDTH  = 6   -- leg B pulse width, raw cycles
REG_CH_BASE     = 7   -- 8 probe channels (i=0..7): enable=+i*3, delay=+i*3+1 (signed), width=+i*3+2

-- Anode channel (SiC half-bridge with fault/reset handling, added 2026-08-18),
-- triggers synchronously from the same event as leg A:
REG_ANODE_ENABLE   = 31  -- 0/1, anode channel master on/off
REG_ANODE_PERIOD   = 32  -- anode PWM burst period, raw cycles
REG_ANODE_DELAY    = 33  -- signed delay of the burst start relative to the common trigger, raw cycles
REG_ANODE_DURATION = 34  -- burst duration, raw cycles
REG_ANODE_DEADTIME = 35  -- ADDITION on top of the 300ns hardwired floor, raw cycles, UNSIGNED.
                         --   Not an absolute value — 0 means "floor only, no addition".
                         --   The FPGA saturates the resulting deadtime to
                         --   [300ns, 100us] regardless of what's sent here.

NUM_REGS = 36
```

`*_ENABLE` registers: `=1` is true, anything else is false.

### Range limits

- `TIME_WIDTH = 23` bits → `0 .. 8,388,607` raw cycles (~167.77ms) — ceiling
  for `period`/`leg_a_width`/`leg_b_width`/`anode_duration`/`anode_period`.
- `MAX_PERIOD_CYCLES = 8,338,607` cycles (~166.77ms) — the safe maximum
  specifically for `REG_PERIOD`, leaving a 25,000-cycle (500us) margin on
  each side of the window.
- Signed delays (`REG_LEGB_DELAY`, probe channel `delay`, `REG_ANODE_DELAY`)
  range from `-25,000` cycles (-500us) up to `period + 25,000` cycles. The
  FPGA does not validate or clamp these — the firmware clamps in
  `menu_reg_limits()` before sending.
- `REG_ANODE_DEADTIME` is edited on the STM32 side within a much tighter,
  UI-only range (see below) — the FPGA's own [300ns, 100us] saturation is a
  separate, independent safety backstop.

## Menu / UI

`hipims_menu.c` drives a 37-item flat list on the 16×2 LCD: the 36 real
registers plus two UI-only entries (`STEP`, `ERRORS`). Turning the encoder
moves the selection or, while editing, changes the value by the current step;
clicking the button enters/confirms edit mode.

- **STEP** — sets the editing step size for every time-kind register
  (`20ns / 1us / 10us / 100us / 1ms`). A dedicated menu item rather than a
  fixed constant, since dialing a coarse register (period, ~ms range) and a
  fine one (a few hundred ns) with the same step size is impractical.
  RAM-only, not persisted.
- **ERRORS** — shows `A: OK/FLT  B: OK/FLT` from the live status-byte fault
  bits. Clicking it does **not** enter edit mode — it triggers a manual fault
  reset (see below).
- Every real register displays as `us.nnn` (integer-only, no floats — 3
  fractional digits of a microsecond is exactly a nanosecond, and 1 raw
  cycle always divides evenly into whole nanoseconds), except:
  - `ENABLE`/`START`/`LEGB_ENABLE`/`ANODE_ENABLE`/probe `ENABLE` — shown as a
    bare `0`/`1`.
  - `ANODE_DEADTIME` — shown as a plain integer nanosecond value (e.g.
    `300ns`), since `us.nnn` formatting reads badly for values this small.
    It also gets its own **fixed 20ns (1 raw cycle) editing step and a
    dedicated 0–2000ns clamp**, independent of the global STEP setting — the
    global step is us-scale and would blow straight past this register's
    entire usable range in a single click. The 2000ns ceiling was
    deliberately set above the ~1us "classic" deadtime figure for
    slower silicon (IR-class MOSFET/driver) prototypes, since the exact
    hardware isn't fixed yet; SiC + fast gate drivers (e.g. UCC21750-class)
    typically need far less, closer to the FPGA's 300ns floor.
- On edit confirm, the value is written to flash (`storage_set`) and sent
  over SPI (`hipims_spi_write_reg`). If the SPI write fails after all
  retries, the LCD shows `SPI FAIL` for ~800ms.
- At boot (`menu_init`), every stored register value is resent to the FPGA,
  and the same full resend repeats every ~1.5s from `menu_tick()`
  (`menu_resend_all_regs()`). The FPGA has no non-volatile memory of its own
  for registers — a standalone FPGA reconfigure (reflash via Programmer, or
  power-cycling just the FPGA board) silently zeroes all 36 registers
  without the STM32 knowing, since it only saw this at its own boot. Without
  the periodic resend this looks like "SPI is fine (ACK), but no
  generation" and is nasty to debug blind — found during 2026-08-19
  bring-up. Safe to do on a timer: `REG_ENABLE` only clears the anode fault
  latch on a `0→1` *edge*, not a level, so repeatedly resending the same `1`
  never re-triggers it; every other register is a plain level with no side
  effect from being rewritten to its current value.

## Fault handling — manual reset only

**Policy, set explicitly by the project owner and hardwired into the FPGA
design: after a fault, generation must stop, and restarting it is always an
explicit STM32-initiated action — never automatic.**

This superseded an earlier auto-reset design that was built and then fully
discarded once the actual constraint became clear: the FPGA's fault latch
used to self-clear after a ~102us cooldown, but the STM32 finds out about a
fault via SPI orders of magnitude slower than that — so the generator could
silently restart before the firmware even knew a fault had happened. The
latch no longer self-clears; it clears **only** on a `REG_ENABLE` `0→1`
edge, which only the firmware can produce.

`hipims_fault_tick()` polls every ~200ms (every 20th 10ms main-loop tick) by
re-writing `REG_START` at its current value purely to get a fresh status
byte back — deliberately not `REG_ENABLE`, so routine polling can never
accidentally produce the edge that clears the latch. Fault bits are read
from the status byte **unconditionally**, regardless of whether that
particular write ACKed — the spec is explicit that fault bits are valid
either way, and an earlier bug (fixed) discarded them on NACK.

`hipims_fault_reset()`, triggered by clicking the `ERRORS` menu item, writes
`REG_ENABLE=0`, waits 1ms, then writes `REG_ENABLE=1`. If the physical fault
condition hasn't actually gone away, the latch re-trips immediately and
`fault_a`/`fault_b` reappear on the next poll — that's expected, not a bug.

## Flash storage / wear leveling

Register values persist across power cycles in the last 1KB flash page
(`0x0800FC00`, page 63 of 64), using an append-only 6-byte record log
(`value_lo`, `value_hi`, `addr` — written last, so a power loss mid-write
just leaves a record that reads back as empty rather than corrupt).
`storage_set()` is a no-op if the value hasn't actually changed, and is
called exactly once per finished edit (button click), never per encoder
tick — flash page-erase endurance (~10,000 cycles) is a real constraint, not
a theoretical one. The log compacts (erase + rewrite all 36 current values)
only once the ~170-record page fills up.

## Debug UART (temporary)

Added when hardware bring-up needed live diagnostics beyond what an
oscilloscope alone could tell. USART1, TX-only, LL-based polling
implementation (no printf/retarget). Every SPI attempt is dumped:

```
SPI tx#00 addr=01 val=00000001 crc=65 status=01 ACK
```

View with `pio device monitor` (`monitor_speed = 115200` is already set in
`platformio.ini` — no external terminal app needed). This console was
explicitly instrumental in diagnosing a real FPGA-side VHDL timing bug (see
below) and is flagged here as provisional infrastructure, not a permanent
feature.

## Status

End-to-end verified on real hardware (2026-08-18): encoder → menu → SPI v2
(CRC + ACK, first-attempt success, no retries needed) → FPGA register commit
→ actual pulse generation, confirmed on an oscilloscope for leg A/B and for
the anode channel (burst starting after `ANODE_DELAY`, gated by
`ANODE_ENABLE`/`ANODE_DURATION`), including the visible deadtime gap between
leg A and leg B.

Getting there involved finding and fixing a real bug on the FPGA side: SPI
writes NACKed deterministically despite a CRC-8 that was independently
verified (numerically, byte-at-a-time C vs. bit-serial VHDL) to be correct.
Tracing the VHDL's synchronous-logic timing by hand (not simulation, no
logic analyzer used) found that `spi_slave_regs.vhd` shifted the status byte
left by one extra bit position before the master's first sample of byte 6,
so the real ACK bit landed where the master reads `fault_a`. Documented and
handed off to the FPGA repository
(`HIPIMS_Generator/techdocs/notes/2026-08-18-status-byte-shift-bug.md`)
rather than edited directly here, since the VHDL is a separate project; the
one-line fix (`and bit_cnt /= 0` guard on the shift) was applied there and
confirmed working on real hardware.

Not yet exercised on real hardware: the 8 probe channels individually (menu
items exist, registers are wired, just not tested with real probe load
yet).

## Building and flashing

```
pio run           # build
pio run -t upload # flash via CMSIS-DAP
pio device monitor # debug console, 115200 8N1
```

Requires a CMSIS-DAP probe connected to the Blue Pill's SWD pins (PA13/PA14).
