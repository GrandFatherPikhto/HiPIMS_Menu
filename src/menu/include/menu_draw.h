#ifndef MENU_DRAW_H
#define MENU_DRAW_H

#include <stdint.h>
#include <stdbool.h>

#include "menu_type.h"

bool menu_draw_update(menu_context_t *ctx, menu_id_t id);
void menu_draw_string_fixed_value_cb(menu_context_t *ctx, menu_id_t id);
void hipims_draw_time_value_cb(menu_context_t *ctx, menu_id_t id);
void hipims_draw_deadtime_value_cb(menu_context_t *ctx, menu_id_t id);
void hipims_errors_draw_cb(menu_context_t *ctx, menu_id_t id);

#endif /* MENU_DRAW_H */