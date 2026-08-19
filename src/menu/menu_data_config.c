#include "menu_data_config.h"
#include "menu_config.h"
#include "menu_edit.h"
#include "menu_draw.h"

#include "hipims_spi.h"

static const int32_t s_factors_period[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_leg_a_width[] = { 1, 10, 100, 1000 };
static const int32_t s_factors_leg_b_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_leg_b_width[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch0_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch0_width[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch1_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch1_width[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch2_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch2_width[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch3_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch3_width[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch4_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch4_width[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch5_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch5_width[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch6_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch6_width[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch7_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_ch7_width[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_anode_period[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_anode_delay[] = { 1, 10, 100, 1000, 10000 };
static const int32_t s_factors_anode_duration[] = { 1, 10, 100, 1000, 10000 };

static const char *s_values_str_enable[] = { "Off", "On" };
static const char *s_values_str_start[] = { "Off", "On" };
static const char *s_values_str_leg_b_enable[] = { "Off", "On" };
static const char *s_values_str_ch0_enable[] = { "Off", "On" };
static const char *s_values_str_ch1_enable[] = { "Off", "On" };
static const char *s_values_str_ch2_enable[] = { "Off", "On" };
static const char *s_values_str_ch3_enable[] = { "Off", "On" };
static const char *s_values_str_ch4_enable[] = { "Off", "On" };
static const char *s_values_str_ch5_enable[] = { "Off", "On" };
static const char *s_values_str_ch6_enable[] = { "Off", "On" };
static const char *s_values_str_ch7_enable[] = { "Off", "On" };
static const char *s_values_str_anode_enable[] = { "Off", "On" };

static const menu_node_config_t s_menu_config[] = {
    [MENU_ID_ENABLE] = {        
        .id = MENU_ID_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_enable
        }
    },
    [MENU_ID_START] = {        
        .id = MENU_ID_START,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_start
        }
    },
    [MENU_ID_PERIOD] = {        
        .id = MENU_ID_PERIOD,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8338607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_period
        }
    },
    [MENU_ID_LEG_A_WIDTH] = {        
        .id = MENU_ID_LEG_A_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 4,
            .factors = s_factors_leg_a_width
        }
    },
    [MENU_ID_LEG_B_ENABLE] = {        
        .id = MENU_ID_LEG_B_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_leg_b_enable
        }
    },
    [MENU_ID_LEG_B_DELAY] = {        
        .id = MENU_ID_LEG_B_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_leg_b_delay
        }
    },
    [MENU_ID_LEG_B_WIDTH] = {        
        .id = MENU_ID_LEG_B_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_leg_b_width
        }
    },
    [MENU_ID_CH0_ENABLE] = {        
        .id = MENU_ID_CH0_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_ch0_enable
        }
    },
    [MENU_ID_CH0_DELAY] = {        
        .id = MENU_ID_CH0_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch0_delay
        }
    },
    [MENU_ID_CH0_WIDTH] = {        
        .id = MENU_ID_CH0_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch0_width
        }
    },
    [MENU_ID_CH1_ENABLE] = {        
        .id = MENU_ID_CH1_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_ch1_enable
        }
    },
    [MENU_ID_CH1_DELAY] = {        
        .id = MENU_ID_CH1_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch1_delay
        }
    },
    [MENU_ID_CH1_WIDTH] = {        
        .id = MENU_ID_CH1_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch1_width
        }
    },
    [MENU_ID_CH2_ENABLE] = {        
        .id = MENU_ID_CH2_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_ch2_enable
        }
    },
    [MENU_ID_CH2_DELAY] = {        
        .id = MENU_ID_CH2_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch2_delay
        }
    },
    [MENU_ID_CH2_WIDTH] = {        
        .id = MENU_ID_CH2_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch2_width
        }
    },
    [MENU_ID_CH3_ENABLE] = {        
        .id = MENU_ID_CH3_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_ch3_enable
        }
    },
    [MENU_ID_CH3_DELAY] = {        
        .id = MENU_ID_CH3_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch3_delay
        }
    },
    [MENU_ID_CH3_WIDTH] = {        
        .id = MENU_ID_CH3_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch3_width
        }
    },
    [MENU_ID_CH4_ENABLE] = {        
        .id = MENU_ID_CH4_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_ch4_enable
        }
    },
    [MENU_ID_CH4_DELAY] = {        
        .id = MENU_ID_CH4_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch4_delay
        }
    },
    [MENU_ID_CH4_WIDTH] = {        
        .id = MENU_ID_CH4_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch4_width
        }
    },
    [MENU_ID_CH5_ENABLE] = {        
        .id = MENU_ID_CH5_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_ch5_enable
        }
    },
    [MENU_ID_CH5_DELAY] = {        
        .id = MENU_ID_CH5_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch5_delay
        }
    },
    [MENU_ID_CH5_WIDTH] = {        
        .id = MENU_ID_CH5_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch5_width
        }
    },
    [MENU_ID_CH6_ENABLE] = {        
        .id = MENU_ID_CH6_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_ch6_enable
        }
    },
    [MENU_ID_CH6_DELAY] = {        
        .id = MENU_ID_CH6_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch6_delay
        }
    },
    [MENU_ID_CH6_WIDTH] = {        
        .id = MENU_ID_CH6_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch6_width
        }
    },
    [MENU_ID_CH7_ENABLE] = {        
        .id = MENU_ID_CH7_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_ch7_enable
        }
    },
    [MENU_ID_CH7_DELAY] = {        
        .id = MENU_ID_CH7_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch7_delay
        }
    },
    [MENU_ID_CH7_WIDTH] = {        
        .id = MENU_ID_CH7_WIDTH,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_ch7_width
        }
    },
    [MENU_ID_ANODE_ENABLE] = {        
        .id = MENU_ID_ANODE_ENABLE,
        .category = MENU_CATEGORY_STRING_FIXED,
        .click_cb = string_fixed_click_cyclic_cb,
        .position_cb = string_fixed_position_cyclic_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = menu_draw_string_fixed_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.string_fixed = {
            .default_idx = 0,
            .count = 2,
            .values = s_values_str_anode_enable
        }
    },
    [MENU_ID_ANODE_PERIOD] = {        
        .id = MENU_ID_ANODE_PERIOD,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_anode_period
        }
    },
    [MENU_ID_ANODE_DELAY] = {        
        .id = MENU_ID_ANODE_DELAY,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = -25000,
            .max = 8413607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_anode_delay
        }
    },
    [MENU_ID_ANODE_DURATION] = {        
        .id = MENU_ID_ANODE_DURATION,
        .category = MENU_CATEGORY_DWORD_FACTOR,
        .click_cb = dword_factor_click_cyclic_factor_cb,
        .position_cb = dword_factor_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_time_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_factor = {
            .default_value = 0,
            .min = 0,
            .max = 8388607,
            .step = 1,
            .default_idx = 0,
            .count = 5,
            .factors = s_factors_anode_duration
        }
    },
    [MENU_ID_ANODE_DEADTIME] = {        
        .id = MENU_ID_ANODE_DEADTIME,
        .category = MENU_CATEGORY_DWORD_SIMPLE,
        .click_cb = NULL,
        .position_cb = dword_simple_position_limit_cb,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_draw_deadtime_value_cb,
        .event_cb = hipims_on_value_changed,
        .data.dword_simple = {
            .default_value = 0,
            .step = 1,
            .min = 0,
            .max = 100
        }
    },
    [MENU_ID_ERRORS] = {        
        .id = MENU_ID_ERRORS,
        .category = MENU_CATEGORY_CALLBACK_CALLBACK,
        .click_cb = hipims_fault_reset_cb,
        .position_cb = NULL,
        .double_click_cb = NULL,
        .long_click_cb = NULL,
        .draw_value_cb = hipims_errors_draw_cb,
        .event_cb = NULL,
        .data.stub_config = {}
    },
};


const menu_node_config_t *menu_data_get_config(void) {
    return s_menu_config;
}
