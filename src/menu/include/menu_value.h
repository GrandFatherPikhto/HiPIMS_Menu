#ifndef MENU_VALUE_H
#define MENU_VALUE_H

#include <stdint.h>
#include <stdbool.h>

#include "menu_type.h"

typedef struct stub_value_t {} stub_value_t;
typedef struct {
    uint8_t idx;
} string_fixed_value_t;
typedef struct {
    uint8_t idx;
    int32_t value;
} dword_factor_value_t;
typedef struct {
    int32_t value;
} dword_simple_value_t;
typedef struct {
    void *value_ptr;
} callback_callback_value_t;

typedef struct menu_node_value {
    menu_id_t id;
    union {
        stub_value_t stub_value;
        string_fixed_value_t string_fixed;
        dword_factor_value_t dword_factor;
        dword_simple_value_t dword_simple;
        callback_callback_value_t callback_callback;
    } data;
} menu_node_value_t;

#endif // MENU_VALUE_H