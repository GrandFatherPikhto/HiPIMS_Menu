#include "hipims_menu.h"

#include <string.h>

#include "stm32f1xx_ll_utils.h"

#include "hipims_encoder.h"
#include "hipims_fault.h"
#include "hipims_lcd1602.h"
#include "hipims_spi.h"
#include "hipims_storage.h"

/* Raw TIM2 counts per encoder detent (4 quadrature edges per click in X4
 * mode) — confirmed against the physical encoder: one click = one step. */
#define ENCODER_COUNTS_PER_STEP 4

typedef enum
{
    REG_KIND_BOOL,     /* 0/1 flags */
    REG_KIND_PERIOD,   /* REG_PERIOD */
    REG_KIND_WIDTH,    /* leg/probe pulse widths */
    REG_KIND_DELAY,    /* signed pre/post-trigger delays */
    REG_KIND_DEADTIME, /* ANODE_DEADTIME: tiny unsigned addition on top of a 300ns hw floor */
} reg_kind_t;

/* ANODE_DEADTIME is an addition on top of the 300ns hardwired floor — in
 * practice never more than ~1000ns even for the slowest drivers, per Denis.
 * The global us-scale STEP setting is too coarse to dial this in, so it gets
 * its own fixed 20ns (1 raw cycle) step and a tight max regardless of STEP. */
#define ANODE_DEADTIME_MAX_NS 2000
#define ANODE_DEADTIME_MAX_CYCLES (ANODE_DEADTIME_MAX_NS / 20)

/* Extra UI-only menu slots, past the last real register. */
#define MENU_STEP_ITEM_INDEX   HIPIMS_NUM_REGS       /* editing step size */
#define MENU_ERRORS_ITEM_INDEX (HIPIMS_NUM_REGS + 1) /* fault_a/fault_b status + manual reset on click */
#define MENU_TOTAL_ITEMS       (HIPIMS_NUM_REGS + 2)

static const int32_t s_step_values[] = { 1, 50, 500, 5000, 50000 }; /* raw cycles: 20ns, 1us, 10us, 100us, 1ms */
#define STEP_VALUES_COUNT ((uint8_t)(sizeof(s_step_values) / sizeof(s_step_values[0])))

static uint8_t s_selected = 0;
static bool s_editing = false;
static int32_t s_accum = 0;
static int32_t s_edit_value = 0; /* scratch value while editing a real register; not persisted until confirmed */
static uint8_t s_step_value_idx = 1; /* 50 raw cycles = 1us, matches the previously fixed step; RAM-only */

static int32_t menu_time_step(void)
{
    return s_step_values[s_step_value_idx];
}

static void menu_item_name(uint8_t idx, char *out /* >= 16 bytes */)
{
    static const char *const top_names[HIPIMS_REG_CH_BASE] = {
        "ENABLE", "START", "PERIOD", "LEGA WIDTH",
        "LEGB ENABLE", "LEGB DELAY", "LEGB WIDTH",
    };
    static const char *const anode_names[5] = {
        "ANODE ENABLE", "ANODE PERIOD", "ANODE DELAY", "ANODE DURATION", "ANODE DEADTIME",
    };

    if (idx == MENU_STEP_ITEM_INDEX)
    {
        strcpy(out, "STEP");
        return;
    }

    if (idx == MENU_ERRORS_ITEM_INDEX)
    {
        strcpy(out, "ERRORS");
        return;
    }

    if (idx < HIPIMS_REG_CH_BASE)
    {
        strcpy(out, top_names[idx]);
        return;
    }

    if (idx >= HIPIMS_REG_ANODE_BASE)
    {
        strcpy(out, anode_names[idx - HIPIMS_REG_ANODE_BASE]);
        return;
    }

    static const char *const field_names[3] = { "ENABLE", "DELAY", "WIDTH" };
    uint8_t rel   = (uint8_t)(idx - HIPIMS_REG_CH_BASE);
    uint8_t ch    = (uint8_t)(rel / 3);
    uint8_t field = (uint8_t)(rel % 3);

    out[0] = 'C';
    out[1] = 'H';
    out[2] = (char)('0' + ch);
    out[3] = ' ';
    out[4] = '\0';
    strcat(out, field_names[field]);
}

static reg_kind_t menu_reg_kind(uint8_t idx)
{
    switch (idx)
    {
    case HIPIMS_REG_ENABLE:
    case HIPIMS_REG_START:
    case HIPIMS_REG_LEGB_ENABLE:
        return REG_KIND_BOOL;
    case HIPIMS_REG_PERIOD:
        return REG_KIND_PERIOD;
    case HIPIMS_REG_LEGA_WIDTH:
    case HIPIMS_REG_LEGB_WIDTH:
        return REG_KIND_WIDTH;
    case HIPIMS_REG_LEGB_DELAY:
        return REG_KIND_DELAY;
    case HIPIMS_REG_ANODE_ENABLE:
        return REG_KIND_BOOL;
    case HIPIMS_REG_ANODE_PERIOD:
    case HIPIMS_REG_ANODE_DURATION:
        return REG_KIND_WIDTH;
    case HIPIMS_REG_ANODE_DEADTIME:
        return REG_KIND_DEADTIME;
    case HIPIMS_REG_ANODE_DELAY:
        return REG_KIND_DELAY;
    default:
    {
        uint8_t field = (uint8_t)((idx - HIPIMS_REG_CH_BASE) % 3);
        if (field == 0) { return REG_KIND_BOOL; }  /* CHx ENABLE */
        if (field == 1) { return REG_KIND_DELAY; } /* CHx DELAY */
        return REG_KIND_WIDTH;                     /* CHx WIDTH */
    }
    }
}

/* Editing step size and clamp range for the register's field kind. Delay
 * bounds use the fixed worst-case period rather than the currently-set
 * REG_PERIOD value — matches the FPGA's own lack of cross-register
 * validation, see techdocs/notes/handoff.md. */
static void menu_reg_limits(uint8_t idx, int32_t *out_min, int32_t *out_max, int32_t *out_step)
{
    switch (menu_reg_kind(idx))
    {
    case REG_KIND_BOOL:
        *out_min = 0;
        *out_max = 1;
        *out_step = 1;
        break;
    case REG_KIND_PERIOD:
        *out_min = 0;
        *out_max = HIPIMS_MAX_PERIOD_CYCLES;
        *out_step = menu_time_step();
        break;
    case REG_KIND_WIDTH:
        *out_min = 0;
        *out_max = HIPIMS_TIME_WIDTH_MAX;
        *out_step = menu_time_step();
        break;
    case REG_KIND_DEADTIME:
        *out_min = 0;
        *out_max = ANODE_DEADTIME_MAX_CYCLES;
        *out_step = 1; /* fixed 20ns — global STEP would overshoot this whole range in one click */
        break;
    case REG_KIND_DELAY:
    default:
        *out_min = -HIPIMS_MARGIN_CYCLES;
        *out_max = HIPIMS_TIME_WIDTH_MAX + HIPIMS_MARGIN_CYCLES;
        *out_step = menu_time_step();
        break;
    }
}

static void append_uint(char *buf, uint8_t *pos, uint32_t value, uint8_t min_digits)
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

/* Raw register cycles (1 LSB = 20ns) as "us.nnn" — full nanosecond precision
 * without floats, since 3 fractional digits of a microsecond is exactly a
 * nanosecond and 1 raw cycle always divides evenly into whole nanoseconds. */
static void format_us(int32_t raw_cycles, char *out)
{
    bool neg = (raw_cycles < 0);
    uint32_t mag = neg ? (uint32_t)(-raw_cycles) : (uint32_t)raw_cycles;
    uint32_t ns = mag * 20u;

    uint8_t pos = 0;
    if (neg) { out[pos++] = '-'; }
    append_uint(out, &pos, ns / 1000u, 1);
    out[pos++] = '.';
    append_uint(out, &pos, ns % 1000u, 3);
    out[pos] = '\0';
}

/* ANODE DEADTIME lives in the tens-to-hundreds-of-ns range (300ns hardwired
 * floor, saturates at 100us) — "0.300us" reads worse than "300ns" for values
 * this small, so give it its own plain-nanosecond display. Register is
 * unsigned, no sign to handle. */
static void format_ns(int32_t raw_cycles, char *out)
{
    uint32_t ns = (uint32_t)raw_cycles * 20u;
    uint8_t pos = 0;
    append_uint(out, &pos, ns, 1);
    out[pos] = '\0';
    strcat(out, "ns");
}

static void menu_draw(void)
{
    char name[16];
    menu_item_name(s_selected, name);

    char row0[17];
    memset(row0, ' ', 16);
    row0[16] = '\0';
    size_t name_len = strlen(name);
    if (name_len > 16) { name_len = 16; }
    memcpy(row0, name, name_len);

    char valstr[14];
    if (s_selected == MENU_STEP_ITEM_INDEX)
    {
        format_us(menu_time_step(), valstr);
        strcat(valstr, "us");
    }
    else if (s_selected == MENU_ERRORS_ITEM_INDEX)
    {
        strcpy(valstr, "A:");
        strcat(valstr, hipims_fault_a() ? "FLT" : "OK");
        strcat(valstr, " B:");
        strcat(valstr, hipims_fault_b() ? "FLT" : "OK");
    }
    else
    {
        int32_t value = s_editing ? s_edit_value : storage_get(s_selected);

        if (menu_reg_kind(s_selected) == REG_KIND_BOOL)
        {
            valstr[0] = (char)('0' + (value ? 1 : 0));
            valstr[1] = '\0';
        }
        else if (s_selected == HIPIMS_REG_ANODE_DEADTIME)
        {
            format_ns(value, valstr);
        }
        else
        {
            format_us(value, valstr);
            strcat(valstr, "us");
        }
    }

    char row1[17];
    memset(row1, ' ', 16);
    row1[16] = '\0';
    row1[0] = s_editing ? '>' : ' ';
    size_t val_len = strlen(valstr);
    if (val_len > 14) { val_len = 14; }
    memcpy(&row1[2], valstr, val_len);

    lcd1602_set_cursor(0, 0);
    lcd1602_print(row0);
    lcd1602_set_cursor(0, 1);
    lcd1602_print(row1);
}

/* The FPGA has no non-volatile memory of its own for regs — a standalone
 * FPGA reconfigure (reflash via Programmer, power-cycle of just the FPGA
 * board) silently zeroes all 36 registers without the STM32 knowing, since
 * it only sees this at its own boot. Looks like "SPI is fine (ACK), but no
 * generation" and is nasty to debug blind. Safe to resend on a timer:
 * REG_ENABLE only clears the fault latch on a 0->1 *edge*, not a level, so
 * resending the same 1 repeatedly does not re-trigger it; every other
 * register is a plain level with no side effect from being rewritten. */
static void menu_resend_all_regs(void)
{
    for (uint8_t addr = 0; addr < HIPIMS_NUM_REGS; addr++)
    {
        hipims_spi_write_reg(addr, storage_get(addr));
    }
}

void menu_init(void)
{
    storage_init();
    menu_resend_all_regs();
    encoder_init();
    menu_draw();
}

#define REG_RESYNC_INTERVAL_TICKS 150 /* ~1.5s at the 10ms main-loop cadence */
static uint16_t s_resync_tick_count = 0;

void menu_tick(void)
{
    s_resync_tick_count++;
    if (s_resync_tick_count >= REG_RESYNC_INTERVAL_TICKS)
    {
        s_resync_tick_count = 0;
        menu_resend_all_regs();
    }

    s_accum += encoder_read_delta();
    int32_t steps = s_accum / ENCODER_COUNTS_PER_STEP;
    s_accum -= steps * ENCODER_COUNTS_PER_STEP;

    bool changed = false;

    if (s_editing && s_selected == MENU_STEP_ITEM_INDEX)
    {
        if (steps != 0)
        {
            int32_t next = (int32_t)s_step_value_idx + steps;
            if (next < 0) { next = 0; }
            if (next > (int32_t)(STEP_VALUES_COUNT - 1)) { next = (int32_t)(STEP_VALUES_COUNT - 1); }
            s_step_value_idx = (uint8_t)next;
            changed = true;
        }

        if (encoder_button_clicked())
        {
            s_editing = false; /* UI-only setting, nothing to send over SPI */
            changed = true;
        }
    }
    else if (s_editing)
    {
        if (steps != 0)
        {
            int32_t min, max, step;
            menu_reg_limits(s_selected, &min, &max, &step);

            int32_t value = s_edit_value + steps * step;
            if (value < min) { value = min; }
            if (value > max) { value = max; }
            s_edit_value = value;
            changed = true;
        }

        if (encoder_button_clicked())
        {
            storage_set(s_selected, s_edit_value); /* one flash write per finished edit, not per tick */
            bool ok = hipims_spi_write_reg(s_selected, s_edit_value);
            s_editing = false;
            changed = true;

            if (!ok)
            {
                lcd1602_set_cursor(0, 1);
                lcd1602_print("SPI FAIL        ");
                LL_mDelay(800);
            }
        }
    }
    else
    {
        if (steps != 0)
        {
            int32_t next = (int32_t)s_selected + steps;
            if (next < 0) { next = 0; }
            if (next > (int32_t)(MENU_TOTAL_ITEMS - 1)) { next = MENU_TOTAL_ITEMS - 1; }
            s_selected = (uint8_t)next;
            changed = true;
        }

        if (encoder_button_clicked())
        {
            if (s_selected == MENU_ERRORS_ITEM_INDEX)
            {
                bool ok = hipims_fault_reset();

                if (!ok)
                {
                    lcd1602_set_cursor(0, 1);
                    lcd1602_print("SPI FAIL        ");
                    LL_mDelay(800);
                }
                /* If the fault is still physically present it will already
                 * show up again in hipims_fault_a()/_b() on the next tick —
                 * that is expected, not a bug. */
            }
            else
            {
                if (s_selected != MENU_STEP_ITEM_INDEX)
                {
                    s_edit_value = storage_get(s_selected);
                }
                s_editing = true;
            }
            changed = true;
        }
    }

    if (changed)
    {
        menu_draw();
    }
}
