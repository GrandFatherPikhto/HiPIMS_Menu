#ifndef MENU_EDIT_H
#define MENU_EDIT_H

#include <stdint.h>
#include <stdbool.h>

#include "menu_type.h"

// Базовые структуры (только forward declarations)
typedef struct menu_context menu_context_t;
typedef struct menu_node menu_node_t;
typedef struct menu_node_config menu_node_config_t;
typedef struct menu_node_value menu_node_value_t;
typedef struct menu_callback menu_callback_t;

void string_fixed_click_cyclic_cb(menu_context_t *ctx, menu_id_t id);
void string_fixed_position_cyclic_cb(menu_context_t *ctx, menu_id_t id, int8_t delta);
void hipims_on_value_changed(menu_context_t *ctx, menu_id_t id, menu_event_t event);
void dword_factor_click_cyclic_factor_cb(menu_context_t *ctx, menu_id_t id);
void dword_factor_position_limit_cb(menu_context_t *ctx, menu_id_t id, int8_t delta);
void dword_simple_position_limit_cb(menu_context_t *ctx, menu_id_t id, int8_t delta);
void hipims_fault_reset_cb(menu_context_t *ctx, menu_id_t id);

#endif /* MENU_EDIT_H */