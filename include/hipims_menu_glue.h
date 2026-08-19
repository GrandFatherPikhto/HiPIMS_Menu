#ifndef HIPIMS_MENU_GLUE_H
#define HIPIMS_MENU_GLUE_H

#include "menu_context.h"
#include "menu_type.h"

/* ============================================================================
 * Glue callbacks wired into the generated menu (src/menu/).
 *
 * The generated menu module is produced by MenuCraft
 * (D:\Projects\Python\MenuCraft) and must NOT be hand-edited in this repo —
 * any change is overwritten by the next regeneration. The generated headers
 * (menu_draw.h/menu_edit.h) currently declare the hipims_* functions
 * themselves, which inverts the dependency (menu -> app); that is accepted
 * for now and tracked as a MenuCraft change in
 * techdocs/notes/menucraft-sync.md. This header documents the canonical
 * app-side implementations in hipims_menu_glue.c.
 * ========================================================================== */

/* Shared event_cb for every register-backed leaf. Fires on
 * MENU_EVENT_STOP_EDIT — the moment the operator backs out of editing a
 * field — and forwards the final value to flash + SPI, exactly once per
 * finished edit. */
void hipims_on_value_changed(menu_context_t *ctx, menu_id_t id, menu_event_t event);

/* draw_value_cb for time-kind registers (period, widths, delays, duration):
 * raw FPGA cycles rendered as "us.nnn us <step>". */
void hipims_draw_time_value_cb(menu_context_t *ctx, menu_id_t id);

/* draw_value_cb for anode_deadtime: raw cycles rendered as a plain ns value. */
void hipims_draw_deadtime_value_cb(menu_context_t *ctx, menu_id_t id);

/* draw_value_cb for the ERRORS node: "A:OK/FLT B:OK/FLT" + " S!" if any SPI
 * write has failed since boot/last reset. */
void hipims_errors_draw_cb(menu_context_t *ctx, menu_id_t id);

/* click_cb for the ERRORS node: performs the manual fault reset and clears
 * the SPI failure counters. NOTE: the generated navigation layer fires
 * click_cb only while already in edit mode, so the reset currently takes two
 * presses (enter to edit, enter again). Making ERRORS a true one-press
 * "action leaf" requires a MenuCraft change — see
 * techdocs/notes/menucraft-sync.md. */
void hipims_fault_reset_cb(menu_context_t *ctx, menu_id_t id);

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
