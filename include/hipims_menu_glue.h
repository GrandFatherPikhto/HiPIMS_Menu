#ifndef HIPIMS_MENU_GLUE_H
#define HIPIMS_MENU_GLUE_H

#include "menu_context.h"
#include "menu_type.h"

/* Shared event_cb for every register-backed leaf in menu/hipims.yaml (see
 * MenuCraft, D:\Projects\Python\MenuCraft). Fires on MENU_EVENT_STOP_EDIT —
 * the moment the operator backs out of editing a field — and forwards the
 * final value to flash + SPI, exactly once per finished edit. */
void hipims_on_value_changed(menu_context_t *ctx, menu_id_t id, menu_event_t event);

/* The generated menu module owns its own in-RAM ctx->values, seeded at
 * compile time from menu/hipims.yaml's `default:`/`default_idx:` (all 0) —
 * it knows nothing about the flash-persisted values in hipims_storage.c.
 * Call this once at boot, after storage_init() and menu_init(), to copy
 * every stored register value into ctx->values so the LCD shows what's
 * actually on the FPGA instead of the compiled-in defaults. */
void hipims_menu_glue_load_from_storage(void);

/* Resends every one of the 36 registers to the FPGA (storage_get -> SPI).
 * Called once at boot and periodically thereafter — see
 * hipims_menu_glue_tick(). The FPGA has no non-volatile memory of its own
 * for registers, so a standalone FPGA reconfigure silently zeroes them
 * without the STM32 knowing; see README.md "Status" section. */
void hipims_menu_glue_resend_all(void);

/* Call once per main-loop tick (~10ms cadence). Re-runs
 * hipims_menu_glue_resend_all() roughly every 1.5s. */
void hipims_menu_glue_tick(void);

/* Runs menu_update() and, if the display actually changed, pushes
 * menu_title_buf()/menu_value_buf() to the physical 16x2 LCD — padded to
 * exactly 16 columns each, since the generated buffers are zero-filled past
 * their real content (a branch node's value_buf is just ">\0...") and would
 * otherwise leave stale characters from a longer previous line on screen. */
void hipims_menu_glue_draw(void);

#endif /* HIPIMS_MENU_GLUE_H */
