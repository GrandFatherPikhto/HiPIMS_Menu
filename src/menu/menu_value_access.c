#include "menu_value_access.h"
#include "menu_config.h"
#include "menu_value.h"
#include "menu_context.h"

int32_t menu_get_int32(menu_context_t *ctx, menu_id_t id) {
    if (ctx == NULL || id >= MENU_ID_COUNT)
        return 0;

    switch (ctx->configs[id].category) {
    case MENU_CATEGORY_STRING_FIXED:
    {
        const string_fixed_config_t *cfg = &ctx->configs[id].data.string_fixed;
        uint8_t idx = ctx->values[id].data.string_fixed.idx;
        return cfg->raw_values ? cfg->raw_values[idx] : (int32_t)idx;
    }
    case MENU_CATEGORY_DWORD_FACTOR:
        return (int32_t)ctx->values[id].data.dword_factor.value;
    case MENU_CATEGORY_DWORD_SIMPLE:
        return (int32_t)ctx->values[id].data.dword_simple.value;
    default:
        return 0;
    }
}

void menu_set_int32(menu_context_t *ctx, menu_id_t id, int32_t value) {
    if (ctx == NULL || id >= MENU_ID_COUNT)
        return;

    switch (ctx->configs[id].category) {
    case MENU_CATEGORY_STRING_FIXED:
    {
        const string_fixed_config_t *cfg = &ctx->configs[id].data.string_fixed;
        if (cfg->raw_values == NULL) {
            ctx->values[id].data.string_fixed.idx = (uint8_t)value;
            break;
        }
        for (uint8_t i = 0; i < cfg->count; i++) {
            if (cfg->raw_values[i] == value) {
                ctx->values[id].data.string_fixed.idx = i;
                break;
            }
        }
    }
        break;
    case MENU_CATEGORY_DWORD_FACTOR:
        ctx->values[id].data.dword_factor.value = (int32_t)value;
        break;
    case MENU_CATEGORY_DWORD_SIMPLE:
        ctx->values[id].data.dword_simple.value = (int32_t)value;
        break;
    default:
        break;
    }
}

uint32_t menu_get_uint32(menu_context_t *ctx, menu_id_t id) {
    if (ctx == NULL || id >= MENU_ID_COUNT)
        return 0;

    switch (ctx->configs[id].category) {
    case MENU_CATEGORY_STRING_FIXED:
    {
        const string_fixed_config_t *cfg = &ctx->configs[id].data.string_fixed;
        uint8_t idx = ctx->values[id].data.string_fixed.idx;
        return cfg->raw_values ? (uint32_t)cfg->raw_values[idx] : (uint32_t)idx;
    }
    case MENU_CATEGORY_DWORD_FACTOR:
        return (uint32_t)ctx->values[id].data.dword_factor.value;
    case MENU_CATEGORY_DWORD_SIMPLE:
        return (uint32_t)ctx->values[id].data.dword_simple.value;
    default:
        return 0;
    }
}

void menu_set_uint32(menu_context_t *ctx, menu_id_t id, uint32_t value) {
    if (ctx == NULL || id >= MENU_ID_COUNT)
        return;

    switch (ctx->configs[id].category) {
    case MENU_CATEGORY_STRING_FIXED:
    {
        const string_fixed_config_t *cfg = &ctx->configs[id].data.string_fixed;
        if (cfg->raw_values == NULL) {
            ctx->values[id].data.string_fixed.idx = (uint8_t)value;
            break;
        }
        for (uint8_t i = 0; i < cfg->count; i++) {
            if ((uint32_t)cfg->raw_values[i] == value) {
                ctx->values[id].data.string_fixed.idx = i;
                break;
            }
        }
    }
        break;
    case MENU_CATEGORY_DWORD_FACTOR:
        ctx->values[id].data.dword_factor.value = (int32_t)value;
        break;
    case MENU_CATEGORY_DWORD_SIMPLE:
        ctx->values[id].data.dword_simple.value = (int32_t)value;
        break;
    default:
        break;
    }
}