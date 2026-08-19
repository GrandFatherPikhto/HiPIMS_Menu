/*
 * Host-side tests for the GENERATED menu logic (src/menu) — navigation,
 * edit state machine, value clamping/wrapping and event callbacks.
 *
 * The menu module is pure C (no hardware), so it runs on any host with a
 * C99 compiler. It must NOT be hand-edited (MenuCraft), which is exactly why
 * these tests matter: they pin down the generated behaviour that the app
 * layer (hipims_menu_glue.c) relies on.
 *
 * NOTE on the navigation sign convention (pinned by these tests):
 * the generated menu treats `delta > 0` as "back" (-> prev) and `delta < 0`
 * as "forward" (-> next). This matches main.c, where positive encoder steps
 * walk the list in the "prev" direction.
 *
 * Build & run with any C99 compiler, e.g.:
 *
 *   gcc -std=c99 -Wall -Wextra -Isrc/menu/include -Iinclude \
 *       test/test_menu/test_menu_logic.c \
 *       src/menu/menu.c src/menu/menu_context.c src/menu/menu_data_config.c \
 *       src/menu/menu_data_context.c src/menu/menu_data_name.c \
 *       src/menu/menu_data_tree.c src/menu/menu_data_value.c \
 *       src/menu/menu_draw.c src/menu/menu_edit.c src/menu/menu_name.c \
 *       src/menu/menu_navigate.c src/menu/menu_value_access.c \
 *       -o test_menu_logic && ./test_menu_logic
 *
 * Return code is the number of failed checks (0 = all passed).
 * Can also be wired into `pio test -e native` if the unity framework is
 * installed (see test/README).
 */

#include <stdio.h>
#include <string.h>

#include "menu.h"
#include "menu_type.h"
#include "menu_context.h"      /* full struct definition for ctx->current */
#include "menu_value.h"        /* menu_node_value_t for the reset */
#include "menu_value_access.h"
#include "menu_data_context.h"

/* Generated headers still declare the app callbacks; include them so the
 * stub definitions below are compile-time checked against those signatures. */
#include "menu_draw.h"
#include "menu_edit.h"

/* ---------------------------------------------------------------------------
 * hipims_* stubs — the generated menu_data_config.c registers these app
 * functions as callbacks; on the host the app layer is replaced by stubs
 * that just record calls.
 * ------------------------------------------------------------------------- */

static int s_value_changed_calls = 0;
static int s_last_value_event = 0;
static int s_fault_reset_calls = 0;

void hipims_on_value_changed(menu_context_t *ctx, menu_id_t id, menu_event_t event)
{
    (void)ctx;
    (void)id;
    s_value_changed_calls++;
    s_last_value_event = (int)event;
}

void hipims_draw_time_value_cb(menu_context_t *ctx, menu_id_t id)
{
    (void)ctx;
    (void)id;
}

void hipims_draw_deadtime_value_cb(menu_context_t *ctx, menu_id_t id)
{
    (void)ctx;
    (void)id;
}

void hipims_errors_draw_cb(menu_context_t *ctx, menu_id_t id)
{
    (void)ctx;
    (void)id;
}

void hipims_fault_reset_cb(menu_context_t *ctx, menu_id_t id)
{
    (void)ctx;
    (void)id;
    s_fault_reset_calls++;
}

/* ---------------------------------------------------------------------------
 * Fixture: re-init the menu, wipe the persistent values array (menu_init()
 * only re-points the context; s_menu_values is static and survives across
 * tests), and reset the recording counters.
 * ------------------------------------------------------------------------- */

static void reset_menu(void)
{
    menu_init();
    menu_context_t *ctx = menu_data_get_context();
    if (ctx != NULL)
    {
        memset(ctx->values, 0, sizeof(ctx->values[0]) * MENU_ID_COUNT);
    }
    s_value_changed_calls = 0;
    s_last_value_event = 0;
    s_fault_reset_calls = 0;
}

/* ---------------------------------------------------------------------------
 * Tiny assert helpers (no external test framework needed).
 * ------------------------------------------------------------------------- */

static int g_failures = 0;

#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
            g_failures++;                                               \
        }                                                               \
    } while (0)

#define CHECK_EQ_INT(a, b)                                              \
    do {                                                                \
        long a_ = (long)(a), b_ = (long)(b);                            \
        if (a_ != b_) {                                                 \
            printf("FAIL %s:%d: %s=%ld, %s=%ld\n",                      \
                   __FILE__, __LINE__, #a, a_, #b, b_);                 \
            g_failures++;                                               \
        }                                                               \
    } while (0)

/* ---------------------------------------------------------------------------
 * Tests
 * ------------------------------------------------------------------------- */

static void test_init_state(void)
{
    reset_menu();
    menu_context_t *ctx = menu_data_get_context();
    CHECK(ctx != NULL);
    CHECK_EQ_INT(MENU_STATE_NAVIGATION, menu_state());
    CHECK_EQ_INT(MENU_ID_ENABLE, ctx->current);
}

/* Root ring: ENABLE <-> START <-> OPTIONS <-> ERRORS <-> ENABLE.
 * delta>0 -> prev, delta<0 -> next. */
static void test_navigation_sibling_ring(void)
{
    reset_menu();
    menu_context_t *ctx = menu_data_get_context();

    menu_position(-1);  /* ENABLE -> next(START) */
    CHECK_EQ_INT(MENU_ID_START, ctx->current);
    menu_position(-1);  /* START -> next(OPTIONS) */
    CHECK_EQ_INT(MENU_ID_OPTIONS, ctx->current);
    menu_position(-1);  /* OPTIONS -> next(ERRORS) */
    CHECK_EQ_INT(MENU_ID_ERRORS, ctx->current);
    menu_position(-1);  /* ERRORS -> next(ENABLE) — wraps around the ring */
    CHECK_EQ_INT(MENU_ID_ENABLE, ctx->current);
    menu_position(1);   /* ENABLE -> prev(ERRORS) — backwards wrap */
    CHECK_EQ_INT(MENU_ID_ERRORS, ctx->current);
}

static void test_enter_leaf_edit_and_commit(void)
{
    reset_menu();
    menu_context_t *ctx = menu_data_get_context();

    /* ENABLE is a string_fixed leaf with event_cb = hipims_on_value_changed. */
    menu_enter();
    CHECK_EQ_INT(MENU_STATE_EDIT, menu_state());
    CHECK_EQ_INT(MENU_EVENT_START_EDIT, s_last_value_event);
    CHECK_EQ_INT(1, s_value_changed_calls);

    menu_back();
    CHECK_EQ_INT(MENU_STATE_NAVIGATION, menu_state());
    CHECK_EQ_INT(MENU_EVENT_STOP_EDIT, s_last_value_event);
    CHECK_EQ_INT(2, s_value_changed_calls);
    (void)ctx;
}

static void test_string_fixed_toggle_and_wrap(void)
{
    reset_menu();
    menu_context_t *ctx = menu_data_get_context();

    menu_enter();                     /* ENABLE -> EDIT */
    CHECK_EQ_INT(0, menu_get_int32(ctx, MENU_ID_ENABLE));

    menu_position(1);                 /* Off -> On */
    CHECK_EQ_INT(1, menu_get_int32(ctx, MENU_ID_ENABLE));

    menu_position(1);                 /* On -> Off (cyclic wrap) */
    CHECK_EQ_INT(0, menu_get_int32(ctx, MENU_ID_ENABLE));

    menu_position(-1);                /* Off -> On (backwards wrap) */
    CHECK_EQ_INT(1, menu_get_int32(ctx, MENU_ID_ENABLE));
}

static void test_dword_factor_edit_and_factor_cycle(void)
{
    reset_menu();
    menu_context_t *ctx = menu_data_get_context();

    /* Navigate: ENABLE -> prev(ERRORS) -> prev(OPTIONS), enter branch ->
     * PERIOD. */
    menu_position(1);
    menu_position(1);
    menu_enter();
    CHECK_EQ_INT(MENU_ID_PERIOD, ctx->current);

    menu_enter();                     /* PERIOD -> EDIT */
    CHECK_EQ_INT(0, menu_get_int32(ctx, MENU_ID_PERIOD));

    menu_position(1);                 /* +1 step at factor idx0 (50 raw cycles) */
    CHECK_EQ_INT(50, menu_get_int32(ctx, MENU_ID_PERIOD));

    menu_enter();                     /* in EDIT, enter = click_cb = cycle factor idx 0->1 */
    menu_position(1);                 /* +1 step at factor idx1 (500) -> 50 + 500 */
    CHECK_EQ_INT(550, menu_get_int32(ctx, MENU_ID_PERIOD));

    menu_position(-1);                /* back down: 550 - 500 = 50 */
    CHECK_EQ_INT(50, menu_get_int32(ctx, MENU_ID_PERIOD));

    menu_back();                      /* commit -> NAVIGATION + STOP_EDIT */
    CHECK_EQ_INT(MENU_STATE_NAVIGATION, menu_state());
    CHECK_EQ_INT(MENU_EVENT_STOP_EDIT, s_last_value_event);
}

static void test_dword_factor_clamp(void)
{
    reset_menu();
    menu_context_t *ctx = menu_data_get_context();

    /* Navigate to Period, enter edit, cycle factor idx 0->1->2->3 (50000). */
    menu_position(1);
    menu_position(1);
    menu_enter();                     /* branch -> PERIOD */
    menu_enter();                     /* PERIOD -> EDIT */
    menu_enter();                     /* idx 0 -> 1 */
    menu_enter();                     /* idx 1 -> 2 */
    menu_enter();                     /* idx 2 -> 3 */

    /* Clamp at min: single negative step from 0 must land on min (0). */
    menu_position(-1);
    CHECK_EQ_INT(0, menu_get_int32(ctx, MENU_ID_PERIOD));

    /* Clamp at max: push past the ceiling in big steps.
     * PERIOD max is 8338607, factor 50000 -> the first two 127-steps already
     * cross it, then every further step must stay pinned at the cap. */
    for (int i = 0; i < 200; i++) { menu_position(127); }
    CHECK_EQ_INT(8338607, menu_get_int32(ctx, MENU_ID_PERIOD));
}

static void test_errors_two_press_reset(void)
{
    reset_menu();
    menu_context_t *ctx = menu_data_get_context();

    menu_position(1);                 /* ENABLE -> prev(ERRORS) */
    CHECK_EQ_INT(MENU_ID_ERRORS, ctx->current);

    /* Generated behaviour (MenuCraft baseline, no hand-edits): a leaf enters
     * EDIT mode first; click_cb fires only on the second press. The app
     * layer would prefer a one-press "action leaf" — tracked in
     * techdocs/notes/menucraft-sync.md. This test pins the current contract. */
    menu_enter();
    CHECK_EQ_INT(MENU_STATE_EDIT, menu_state());
    CHECK_EQ_INT(0, s_fault_reset_calls);

    menu_enter();
    CHECK_EQ_INT(1, s_fault_reset_calls);
}

static void test_redraw_dirty_flow(void)
{
    reset_menu();

    CHECK(menu_ack_redraw() == false);   /* clean context: nothing to draw */
    menu_set_dirty();
    menu_update();
    CHECK(menu_ack_redraw() == true);    /* ack once... */
    CHECK(menu_ack_redraw() == false);   /* ...then it's consumed */
}

static void test_branch_enter_descends(void)
{
    reset_menu();
    menu_context_t *ctx = menu_data_get_context();

    /* ENABLE -> ERRORS -> OPTIONS (branch); entering descends to first child. */
    menu_position(1);
    menu_position(1);
    CHECK_EQ_INT(MENU_ID_OPTIONS, ctx->current);
    menu_enter();
    CHECK_EQ_INT(MENU_ID_PERIOD, ctx->current);

    /* Back on a branch navigates up to the parent (OPTIONS here). */
    menu_back();
    CHECK_EQ_INT(MENU_ID_OPTIONS, ctx->current);
}

int main(void)
{
    test_init_state();
    test_navigation_sibling_ring();
    test_enter_leaf_edit_and_commit();
    test_string_fixed_toggle_and_wrap();
    test_dword_factor_edit_and_factor_cycle();
    test_dword_factor_clamp();
    test_errors_two_press_reset();
    test_redraw_dirty_flow();
    test_branch_enter_descends();

    if (g_failures == 0)
    {
        printf("OK: all menu logic tests passed\n");
        return 0;
    }

    printf("%d FAILED check(s)\n", g_failures);
    return g_failures;
}
