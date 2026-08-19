#include "hipims_menu_glue.h"

#include <string.h>

#include "menu.h"
#include "menu_config.h"
#include "menu_data_context.h"
#include "menu_draw.h"
#include "menu_edit.h"
#include "menu_value.h"
#include "menu_value_access.h"

#include "hipims_fault.h"
#include "hipims_lcd1602.h"
#include "hipims_spi.h"
#include "hipims_storage.h"

/* Every leaf that carries a real register has `event_cb: hipims_on_value_changed`
 * and a `tag: <reg addr>` in menu/hipims.yaml (branches and ERRORS have
 * neither) — so this fires only for genuine register leaves, and menu_get_int32()
 * (generic across string_fixed/dword_factor/dword_simple, keyed off
 * ctx->configs[id].category) reads whichever union member actually applies
 * without this file needing to know or care which. One flash write + one
 * SPI write per finished edit, not per tick — gated on MENU_EVENT_STOP_EDIT.
 *
 * The SPI write's return value is deliberately not acted on here: a NACKed
 * write is counted inside hipims_spi.c (hipims_spi_tx_failures()) and
 * surfaced on the ERRORS screen, and the ~1.5s periodic resend re-pushes the
 * persisted value anyway. Persisting to flash regardless keeps the STM32 the
 * source of truth even if the FPGA is momentarily out of sync. */
void hipims_on_value_changed(menu_context_t *ctx, menu_id_t id, menu_event_t event)
{
    if (event != MENU_EVENT_STOP_EDIT) { return; }

    /* Defensive: the tag comes from menu/hipims.yaml and must match the
     * register map in hipims_spi.h (see techdocs/notes/menucraft-sync.md).
     * A stale tag is better ignored loudly-by-nothing than silently writing
     * to the wrong register. */
    uint32_t tag = ctx->configs[id].tag;
    if (tag >= HIPIMS_NUM_REGS) { return; }

    uint8_t reg_addr = (uint8_t)tag;
    int32_t value = menu_get_int32(ctx, id);

    storage_set(reg_addr, value);
    hipims_spi_write_reg(reg_addr, value);
}

#define HIPIMS_LCD_ROW_LEN 16

static void append_uint32(char *buf, size_t *pos, uint32_t value, uint8_t min_digits)
{
    char tmp[12];
    uint8_t n = 0;
    do
    {
        tmp[n++] = (char)('0' + (value % 10));
        value /= 10;
    } while (value != 0 || n < min_digits);

    while (n > 0) { buf[(*pos)++] = tmp[--n]; }
}

static void append_str(char *buf, size_t *pos, const char *s)
{
    while (*s) { buf[(*pos)++] = *s++; }
}

/* Raw register cycles (1 LSB = 20ns) as "us.nnn" — same integer-only
 * conversion the old hand-written hipims_menu.c used: 3 fractional digits
 * of a microsecond is exactly a nanosecond, and 1 raw cycle always divides
 * evenly into whole nanoseconds. */
static void append_us(char *buf, size_t *pos, int32_t raw_cycles)
{
    bool neg = (raw_cycles < 0);
    uint32_t mag = neg ? (uint32_t)(-raw_cycles) : (uint32_t)raw_cycles;
    uint32_t ns = mag * 20u;

    if (neg) { buf[(*pos)++] = '-'; }
    append_uint32(buf, pos, ns / 1000u, 1);
    buf[(*pos)++] = '.';
    append_uint32(buf, pos, ns % 1000u, 3);
}

/* Compact step-size label for the currently selected factor — "1u"/"100u"/
 * "1m" — so the operator can still see which click-to-cycle scale is
 * active, same information the auto-generated "(x50)" used to carry, just
 * in the same time units as the value instead of a raw cycle-count
 * multiplier. No factor list in menu/hipims.yaml goes below 50 raw cycles
 * (1us) — nanosecond step resolution is reserved for anode_deadtime alone,
 * which has its own dedicated ns display — so there's no ns case to handle
 * here, only us/ms. Every factor is a round number of raw cycles, so
 * integer division is always exact. */
static void append_step_label(char *buf, size_t *pos, int32_t factor_raw_cycles)
{
    uint32_t ns = (uint32_t)factor_raw_cycles * 20u;
    if (ns < 1000000u)
    {
        append_uint32(buf, pos, ns / 1000u, 1);
        buf[(*pos)++] = 'u';
    }
    else
    {
        append_uint32(buf, pos, ns / 1000000u, 1);
        buf[(*pos)++] = 'm';
    }
}

/* Registered directly as draw_value_cb in menu/hipims.yaml for every
 * role: factor time field (period, widths, delays, duration) — MenuCraft's
 * draw.c.jinja now checks function_info.source == "custom" (fixed
 * 2026-08-19) so a custom name here is genuinely declaration-only, no
 * auto-generated body fighting it. */
void hipims_draw_time_value_cb(menu_context_t *ctx, menu_id_t id)
{
    int32_t value = ctx->values[id].data.dword_factor.value;
    uint8_t idx = ctx->values[id].data.dword_factor.idx;
    int32_t factor = ctx->configs[id].data.dword_factor.factors[idx];

    size_t pos = 0;
    append_us(ctx->value_buf, &pos, value);
    ctx->value_buf[pos++] = 'u';
    ctx->value_buf[pos++] = 's';
    ctx->value_buf[pos++] = ' ';
    append_step_label(ctx->value_buf, &pos, factor);
    menu_draw_pad_marker(ctx);
}

/* anode_deadtime is the one time-kind field deliberately NOT shown as
 * us.nnn — its whole range is a few hundred to ~2000ns, where "0.300us"
 * reads worse than "300ns". role: simple, so dword_simple union member. */
void hipims_draw_deadtime_value_cb(menu_context_t *ctx, menu_id_t id)
{
    int32_t value = ctx->values[id].data.dword_simple.value;
    uint32_t ns = (uint32_t)value * 20u;
    size_t pos = 0;
    append_uint32(ctx->value_buf, &pos, ns, 1);
    ctx->value_buf[pos++] = 'n';
    ctx->value_buf[pos++] = 's';
    menu_draw_pad_marker(ctx);
}

void hipims_errors_draw_cb(menu_context_t *ctx, menu_id_t id)
{
    (void)id;
    size_t pos = 0;

    append_str(ctx->value_buf, &pos, "A:");
    append_str(ctx->value_buf, &pos, hipims_fault_a() ? "FLT" : "OK");
    append_str(ctx->value_buf, &pos, " B:");
    append_str(ctx->value_buf, &pos, hipims_fault_b() ? "FLT" : "OK");

    /* Surface the SPI failure counter so a write that never reached the FPGA
     * is visible instead of being silently swallowed (the ~1.5s resend
     * re-pushes it, but the operator should know it happened). */
    if (hipims_spi_tx_failures() != 0)
    {
        append_str(ctx->value_buf, &pos, " S!");
    }

    while (pos < HIPIMS_LCD_ROW_LEN) { ctx->value_buf[pos++] = ' '; }
    ctx->value_buf[HIPIMS_LCD_ROW_LEN] = '\0';
}

/* Bound in the generated menu (menu_data_config.c) as the ERRORS click_cb.
 * The generated navigation layer fires click_cb only while already in edit
 * mode, so a reset currently takes two presses (enter to edit, enter again).
 * Making ERRORS a true one-press "action leaf" requires a MenuCraft change —
 * the generated src/menu/ is not hand-edited here; see
 * techdocs/notes/menucraft-sync.md. */
void hipims_fault_reset_cb(menu_context_t *ctx, menu_id_t id)
{
    (void)ctx;
    (void)id;
    hipims_fault_reset();

    /* The operator just acknowledged the fault state — treat that as the
     * moment to also clear the SPI failure counters, so a stale "S!" from a
     * long-since-healed write doesn't linger forever. */
    hipims_spi_clear_failures();
}

/* Every register-backed node carries a `tag: <reg addr>` (branches and
 * ERRORS are 0) — so a plain sweep over every id, filtered on a non-zero
 * in-range tag, reaches precisely the register leaves without needing to
 * enumerate menu_id_t by name or know the menu's category set. Keying on tag
 * instead of category is robust to MenuCraft adding new value categories. */
void hipims_menu_glue_load_from_storage(void)
{
    menu_context_t *ctx = menu_data_get_context();
    if (ctx == NULL) { return; }

    for (uint8_t id = 0; id < MENU_ID_COUNT; id++)
    {
        uint32_t tag = ctx->configs[id].tag;
        if (tag == 0 || tag >= HIPIMS_NUM_REGS)
        {
            continue;
        }

        menu_set_int32(ctx, (menu_id_t)id, storage_get((uint8_t)tag));
    }
}

#define HIPIMS_MENU_RESYNC_INTERVAL_TICKS 150 /* ~1.5s at the 10ms main-loop cadence */
static uint16_t s_resync_tick_count = 0;

void hipims_menu_glue_resend_all(void)
{
    for (uint8_t addr = 0; addr < HIPIMS_NUM_REGS; addr++)
    {
        hipims_spi_write_reg(addr, storage_get(addr));
    }
}

void hipims_menu_glue_tick(void)
{
    s_resync_tick_count++;
    if (s_resync_tick_count >= HIPIMS_MENU_RESYNC_INTERVAL_TICKS)
    {
        s_resync_tick_count = 0;
        hipims_menu_glue_resend_all();
    }
}

/* Copies up to 16 visible chars from `in` into `out`, space-padded to
 * exactly 16 + '\0' — the generated title_buf/value_buf are only
 * zero-filled past their real content, not space-padded, so printing them
 * as-is would leave stale characters from a longer previous line. */
static void pad_to_lcd_row(const char *in, char out[HIPIMS_LCD_ROW_LEN + 1])
{
    size_t len = strlen(in);
    if (len > HIPIMS_LCD_ROW_LEN) { len = HIPIMS_LCD_ROW_LEN; }
    memcpy(out, in, len);
    for (size_t i = len; i < HIPIMS_LCD_ROW_LEN; i++) { out[i] = ' '; }
    out[HIPIMS_LCD_ROW_LEN] = '\0';
}

void hipims_menu_glue_draw(void)
{
    menu_update();
    if (!menu_ack_redraw()) { return; }

    char row[HIPIMS_LCD_ROW_LEN + 1];

    pad_to_lcd_row(menu_title_buf(), row);
    lcd1602_set_cursor(0, 0);
    lcd1602_print(row);

    pad_to_lcd_row(menu_value_buf(), row);
    lcd1602_set_cursor(0, 1);
    lcd1602_print(row);
}
