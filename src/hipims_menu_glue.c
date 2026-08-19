#include "hipims_menu_glue.h"

#include <stdio.h>
#include <string.h>

#include "menu.h"
#include "menu_config.h"
#include "menu_data_context.h"
#include "menu_draw.h"
#include "menu_edit.h"
#include "menu_value.h"

#include "hipims_fault.h"
#include "hipims_lcd1602.h"
#include "hipims_spi.h"
#include "hipims_storage.h"

/* One flash write + one SPI write per finished edit, not per tick — see
 * MENU_EVENT_STOP_EDIT gate in hipims_on_value_changed() below. */
static void forward(uint8_t reg_addr, int32_t value)
{
    storage_set(reg_addr, value);
    hipims_spi_write_reg(reg_addr, value);
}

void hipims_on_value_changed(menu_context_t *ctx, menu_id_t id, menu_event_t event)
{
    if (event != MENU_EVENT_STOP_EDIT) { return; }

    switch (id)
    {
    case MENU_ID_ENABLE:       forward(HIPIMS_REG_ENABLE, ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_START:        forward(HIPIMS_REG_START, ctx->values[id].data.string_fixed.idx); break;

    case MENU_ID_PERIOD:       forward(HIPIMS_REG_PERIOD, ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_LEG_A_WIDTH:  forward(HIPIMS_REG_LEGA_WIDTH, ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_LEG_B_ENABLE: forward(HIPIMS_REG_LEGB_ENABLE, ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_LEG_B_DELAY:  forward(HIPIMS_REG_LEGB_DELAY, ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_LEG_B_WIDTH:  forward(HIPIMS_REG_LEGB_WIDTH, ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_CH0_ENABLE: forward(HIPIMS_REG_CH_ENABLE(0), ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_CH0_DELAY:  forward(HIPIMS_REG_CH_DELAY(0), ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_CH0_WIDTH:  forward(HIPIMS_REG_CH_WIDTH(0), ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_CH1_ENABLE: forward(HIPIMS_REG_CH_ENABLE(1), ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_CH1_DELAY:  forward(HIPIMS_REG_CH_DELAY(1), ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_CH1_WIDTH:  forward(HIPIMS_REG_CH_WIDTH(1), ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_CH2_ENABLE: forward(HIPIMS_REG_CH_ENABLE(2), ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_CH2_DELAY:  forward(HIPIMS_REG_CH_DELAY(2), ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_CH2_WIDTH:  forward(HIPIMS_REG_CH_WIDTH(2), ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_CH3_ENABLE: forward(HIPIMS_REG_CH_ENABLE(3), ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_CH3_DELAY:  forward(HIPIMS_REG_CH_DELAY(3), ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_CH3_WIDTH:  forward(HIPIMS_REG_CH_WIDTH(3), ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_CH4_ENABLE: forward(HIPIMS_REG_CH_ENABLE(4), ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_CH4_DELAY:  forward(HIPIMS_REG_CH_DELAY(4), ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_CH4_WIDTH:  forward(HIPIMS_REG_CH_WIDTH(4), ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_CH5_ENABLE: forward(HIPIMS_REG_CH_ENABLE(5), ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_CH5_DELAY:  forward(HIPIMS_REG_CH_DELAY(5), ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_CH5_WIDTH:  forward(HIPIMS_REG_CH_WIDTH(5), ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_CH6_ENABLE: forward(HIPIMS_REG_CH_ENABLE(6), ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_CH6_DELAY:  forward(HIPIMS_REG_CH_DELAY(6), ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_CH6_WIDTH:  forward(HIPIMS_REG_CH_WIDTH(6), ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_CH7_ENABLE: forward(HIPIMS_REG_CH_ENABLE(7), ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_CH7_DELAY:  forward(HIPIMS_REG_CH_DELAY(7), ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_CH7_WIDTH:  forward(HIPIMS_REG_CH_WIDTH(7), ctx->values[id].data.dword_factor.value); break;

    case MENU_ID_ANODE_ENABLE:   forward(HIPIMS_REG_ANODE_ENABLE, ctx->values[id].data.string_fixed.idx); break;
    case MENU_ID_ANODE_PERIOD:   forward(HIPIMS_REG_ANODE_PERIOD, ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_ANODE_DELAY:    forward(HIPIMS_REG_ANODE_DELAY, ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_ANODE_DURATION: forward(HIPIMS_REG_ANODE_DURATION, ctx->values[id].data.dword_factor.value); break;
    case MENU_ID_ANODE_DEADTIME: forward(HIPIMS_REG_ANODE_DEADTIME, ctx->values[id].data.dword_simple.value); break;

    default: break; /* branch nodes (OPTIONS/LEG_A/LEG_B/CHANNEL_N/ANODE/ROOT) and ERRORS carry no register */
    }
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

/* Compact step-size label for the currently selected factor — "20n"/"2u"/
 * "100u"/"1m" — so the operator can still see which click-to-cycle scale is
 * active, same information the auto-generated "(x50)" used to carry, just
 * in the same time units as the value instead of a raw cycle-count
 * multiplier. Every factor list in menu/hipims.yaml is a round number of
 * raw cycles, so integer ns/us/ms division is always exact here. */
static void append_step_label(char *buf, size_t *pos, int32_t factor_raw_cycles)
{
    uint32_t ns = (uint32_t)factor_raw_cycles * 20u;
    if (ns < 1000u)
    {
        append_uint32(buf, pos, ns, 1);
        buf[(*pos)++] = 'n';
    }
    else if (ns < 1000000u)
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

/* Mirrors the generated menu_draw_line_marker() (static, not exported):
 * pad to 15 columns, then '*' while editing or '>' while navigating. */
static void draw_value_marker(menu_context_t *ctx, size_t content_len)
{
    size_t pos = content_len;
    while (pos < HIPIMS_LCD_ROW_LEN - 1) { ctx->value_buf[pos++] = ' '; }
    ctx->value_buf[pos] = (char)(ctx->state == MENU_STATE_EDIT ? '*' : '>');
    ctx->value_buf[pos + 1] = '\0';
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
    draw_value_marker(ctx, pos);
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
    draw_value_marker(ctx, pos);
}

void hipims_errors_draw_cb(menu_context_t *ctx, menu_id_t id)
{
    (void)id;
    char line[HIPIMS_LCD_ROW_LEN + 1];
    int len = snprintf(line, sizeof(line), "A:%s B:%s",
                        hipims_fault_a() ? "FLT" : "OK",
                        hipims_fault_b() ? "FLT" : "OK");
    if (len < 0) { len = 0; }
    while (len < HIPIMS_LCD_ROW_LEN) { line[len++] = ' '; } /* no leftover chars from a longer previous line */
    line[HIPIMS_LCD_ROW_LEN] = '\0';

    memcpy(ctx->value_buf, line, sizeof(line));
}

void hipims_fault_reset_cb(menu_context_t *ctx, menu_id_t id)
{
    (void)ctx;
    (void)id;
    hipims_fault_reset();
}

static void load_bool(menu_context_t *ctx, menu_id_t id, uint8_t reg_addr)
{
    ctx->values[id].data.string_fixed.idx = (uint8_t)(storage_get(reg_addr) ? 1u : 0u);
}

static void load_factor(menu_context_t *ctx, menu_id_t id, uint8_t reg_addr)
{
    ctx->values[id].data.dword_factor.value = storage_get(reg_addr);
}

static void load_simple(menu_context_t *ctx, menu_id_t id, uint8_t reg_addr)
{
    ctx->values[id].data.dword_simple.value = storage_get(reg_addr);
}

void hipims_menu_glue_load_from_storage(void)
{
    menu_context_t *ctx = menu_data_get_context();
    if (ctx == NULL) { return; }

    load_bool(ctx, MENU_ID_ENABLE, HIPIMS_REG_ENABLE);
    load_bool(ctx, MENU_ID_START, HIPIMS_REG_START);

    load_factor(ctx, MENU_ID_PERIOD, HIPIMS_REG_PERIOD);
    load_factor(ctx, MENU_ID_LEG_A_WIDTH, HIPIMS_REG_LEGA_WIDTH);

    load_bool(ctx, MENU_ID_LEG_B_ENABLE, HIPIMS_REG_LEGB_ENABLE);
    load_factor(ctx, MENU_ID_LEG_B_DELAY, HIPIMS_REG_LEGB_DELAY);
    load_factor(ctx, MENU_ID_LEG_B_WIDTH, HIPIMS_REG_LEGB_WIDTH);

    load_bool(ctx, MENU_ID_CH0_ENABLE, HIPIMS_REG_CH_ENABLE(0));
    load_factor(ctx, MENU_ID_CH0_DELAY, HIPIMS_REG_CH_DELAY(0));
    load_factor(ctx, MENU_ID_CH0_WIDTH, HIPIMS_REG_CH_WIDTH(0));

    load_bool(ctx, MENU_ID_CH1_ENABLE, HIPIMS_REG_CH_ENABLE(1));
    load_factor(ctx, MENU_ID_CH1_DELAY, HIPIMS_REG_CH_DELAY(1));
    load_factor(ctx, MENU_ID_CH1_WIDTH, HIPIMS_REG_CH_WIDTH(1));

    load_bool(ctx, MENU_ID_CH2_ENABLE, HIPIMS_REG_CH_ENABLE(2));
    load_factor(ctx, MENU_ID_CH2_DELAY, HIPIMS_REG_CH_DELAY(2));
    load_factor(ctx, MENU_ID_CH2_WIDTH, HIPIMS_REG_CH_WIDTH(2));

    load_bool(ctx, MENU_ID_CH3_ENABLE, HIPIMS_REG_CH_ENABLE(3));
    load_factor(ctx, MENU_ID_CH3_DELAY, HIPIMS_REG_CH_DELAY(3));
    load_factor(ctx, MENU_ID_CH3_WIDTH, HIPIMS_REG_CH_WIDTH(3));

    load_bool(ctx, MENU_ID_CH4_ENABLE, HIPIMS_REG_CH_ENABLE(4));
    load_factor(ctx, MENU_ID_CH4_DELAY, HIPIMS_REG_CH_DELAY(4));
    load_factor(ctx, MENU_ID_CH4_WIDTH, HIPIMS_REG_CH_WIDTH(4));

    load_bool(ctx, MENU_ID_CH5_ENABLE, HIPIMS_REG_CH_ENABLE(5));
    load_factor(ctx, MENU_ID_CH5_DELAY, HIPIMS_REG_CH_DELAY(5));
    load_factor(ctx, MENU_ID_CH5_WIDTH, HIPIMS_REG_CH_WIDTH(5));

    load_bool(ctx, MENU_ID_CH6_ENABLE, HIPIMS_REG_CH_ENABLE(6));
    load_factor(ctx, MENU_ID_CH6_DELAY, HIPIMS_REG_CH_DELAY(6));
    load_factor(ctx, MENU_ID_CH6_WIDTH, HIPIMS_REG_CH_WIDTH(6));

    load_bool(ctx, MENU_ID_CH7_ENABLE, HIPIMS_REG_CH_ENABLE(7));
    load_factor(ctx, MENU_ID_CH7_DELAY, HIPIMS_REG_CH_DELAY(7));
    load_factor(ctx, MENU_ID_CH7_WIDTH, HIPIMS_REG_CH_WIDTH(7));

    load_bool(ctx, MENU_ID_ANODE_ENABLE, HIPIMS_REG_ANODE_ENABLE);
    load_factor(ctx, MENU_ID_ANODE_PERIOD, HIPIMS_REG_ANODE_PERIOD);
    load_factor(ctx, MENU_ID_ANODE_DELAY, HIPIMS_REG_ANODE_DELAY);
    load_factor(ctx, MENU_ID_ANODE_DURATION, HIPIMS_REG_ANODE_DURATION);
    load_simple(ctx, MENU_ID_ANODE_DEADTIME, HIPIMS_REG_ANODE_DEADTIME);
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
