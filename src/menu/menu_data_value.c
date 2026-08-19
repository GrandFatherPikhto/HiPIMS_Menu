#include "menu_data_value.h"
#include "menu_value.h"

static menu_node_value_t s_menu_values[] = {
    // enable fixed
    [MENU_ID_ENABLE] = {
        .id = MENU_ID_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // start fixed
    [MENU_ID_START] = {
        .id = MENU_ID_START,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // period factor
    [MENU_ID_PERIOD] = {
        .id = MENU_ID_PERIOD,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // leg_a_width factor
    [MENU_ID_LEG_A_WIDTH] = {
        .id = MENU_ID_LEG_A_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // leg_b_enable fixed
    [MENU_ID_LEG_B_ENABLE] = {
        .id = MENU_ID_LEG_B_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // leg_b_delay factor
    [MENU_ID_LEG_B_DELAY] = {
        .id = MENU_ID_LEG_B_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // leg_b_width factor
    [MENU_ID_LEG_B_WIDTH] = {
        .id = MENU_ID_LEG_B_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch0_enable fixed
    [MENU_ID_CH0_ENABLE] = {
        .id = MENU_ID_CH0_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // ch0_delay factor
    [MENU_ID_CH0_DELAY] = {
        .id = MENU_ID_CH0_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch0_width factor
    [MENU_ID_CH0_WIDTH] = {
        .id = MENU_ID_CH0_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch1_enable fixed
    [MENU_ID_CH1_ENABLE] = {
        .id = MENU_ID_CH1_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // ch1_delay factor
    [MENU_ID_CH1_DELAY] = {
        .id = MENU_ID_CH1_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch1_width factor
    [MENU_ID_CH1_WIDTH] = {
        .id = MENU_ID_CH1_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch2_enable fixed
    [MENU_ID_CH2_ENABLE] = {
        .id = MENU_ID_CH2_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // ch2_delay factor
    [MENU_ID_CH2_DELAY] = {
        .id = MENU_ID_CH2_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch2_width factor
    [MENU_ID_CH2_WIDTH] = {
        .id = MENU_ID_CH2_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch3_enable fixed
    [MENU_ID_CH3_ENABLE] = {
        .id = MENU_ID_CH3_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // ch3_delay factor
    [MENU_ID_CH3_DELAY] = {
        .id = MENU_ID_CH3_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch3_width factor
    [MENU_ID_CH3_WIDTH] = {
        .id = MENU_ID_CH3_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch4_enable fixed
    [MENU_ID_CH4_ENABLE] = {
        .id = MENU_ID_CH4_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // ch4_delay factor
    [MENU_ID_CH4_DELAY] = {
        .id = MENU_ID_CH4_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch4_width factor
    [MENU_ID_CH4_WIDTH] = {
        .id = MENU_ID_CH4_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch5_enable fixed
    [MENU_ID_CH5_ENABLE] = {
        .id = MENU_ID_CH5_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // ch5_delay factor
    [MENU_ID_CH5_DELAY] = {
        .id = MENU_ID_CH5_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch5_width factor
    [MENU_ID_CH5_WIDTH] = {
        .id = MENU_ID_CH5_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch6_enable fixed
    [MENU_ID_CH6_ENABLE] = {
        .id = MENU_ID_CH6_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // ch6_delay factor
    [MENU_ID_CH6_DELAY] = {
        .id = MENU_ID_CH6_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch6_width factor
    [MENU_ID_CH6_WIDTH] = {
        .id = MENU_ID_CH6_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch7_enable fixed
    [MENU_ID_CH7_ENABLE] = {
        .id = MENU_ID_CH7_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // ch7_delay factor
    [MENU_ID_CH7_DELAY] = {
        .id = MENU_ID_CH7_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // ch7_width factor
    [MENU_ID_CH7_WIDTH] = {
        .id = MENU_ID_CH7_WIDTH,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // anode_enable fixed
    [MENU_ID_ANODE_ENABLE] = {
        .id = MENU_ID_ANODE_ENABLE,
        .data.string_fixed = {
            .idx = 0
        }
    },
    // anode_period factor
    [MENU_ID_ANODE_PERIOD] = {
        .id = MENU_ID_ANODE_PERIOD,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // anode_delay factor
    [MENU_ID_ANODE_DELAY] = {
        .id = MENU_ID_ANODE_DELAY,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // anode_duration factor
    [MENU_ID_ANODE_DURATION] = {
        .id = MENU_ID_ANODE_DURATION,
        .data.dword_factor = {
            .idx = 0,
            .value = 0
        }
    },
    // anode_deadtime simple
    [MENU_ID_ANODE_DEADTIME] = {
        .id = MENU_ID_ANODE_DEADTIME,
        .data.dword_simple = {
            .value = 0
        }
    },
    // errors callback
    [MENU_ID_ERRORS] = {
        .id = MENU_ID_ERRORS,
        .data.callback_callback = {
            .value_ptr = 0
        }
    },
};

menu_node_value_t *menu_data_get_values(void) {
    return s_menu_values;
}