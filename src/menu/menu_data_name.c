#include "menu_data_name.h"
#include "menu_name.h"

static const menu_node_name_t s_menu_id_names[] = {
    [MENU_ID_ROOT] = {MENU_ID_ROOT, "MENU_ID_ROOT"},
    [MENU_ID_ENABLE] = {
            .id = MENU_ID_ENABLE,
            .name = "MENU_ID_ENABLE",
    },
    [MENU_ID_START] = {
            .id = MENU_ID_START,
            .name = "MENU_ID_START",
    },
    [MENU_ID_OPTIONS] = {
            .id = MENU_ID_OPTIONS,
            .name = "MENU_ID_OPTIONS",
    },
    [MENU_ID_PERIOD] = {
            .id = MENU_ID_PERIOD,
            .name = "MENU_ID_PERIOD",
    },
    [MENU_ID_LEG_A] = {
            .id = MENU_ID_LEG_A,
            .name = "MENU_ID_LEG_A",
    },
    [MENU_ID_LEG_A_WIDTH] = {
            .id = MENU_ID_LEG_A_WIDTH,
            .name = "MENU_ID_LEG_A_WIDTH",
    },
    [MENU_ID_LEG_B] = {
            .id = MENU_ID_LEG_B,
            .name = "MENU_ID_LEG_B",
    },
    [MENU_ID_LEG_B_ENABLE] = {
            .id = MENU_ID_LEG_B_ENABLE,
            .name = "MENU_ID_LEG_B_ENABLE",
    },
    [MENU_ID_LEG_B_DELAY] = {
            .id = MENU_ID_LEG_B_DELAY,
            .name = "MENU_ID_LEG_B_DELAY",
    },
    [MENU_ID_LEG_B_WIDTH] = {
            .id = MENU_ID_LEG_B_WIDTH,
            .name = "MENU_ID_LEG_B_WIDTH",
    },
    [MENU_ID_CHANNEL_0] = {
            .id = MENU_ID_CHANNEL_0,
            .name = "MENU_ID_CHANNEL_0",
    },
    [MENU_ID_CH0_ENABLE] = {
            .id = MENU_ID_CH0_ENABLE,
            .name = "MENU_ID_CH0_ENABLE",
    },
    [MENU_ID_CH0_DELAY] = {
            .id = MENU_ID_CH0_DELAY,
            .name = "MENU_ID_CH0_DELAY",
    },
    [MENU_ID_CH0_WIDTH] = {
            .id = MENU_ID_CH0_WIDTH,
            .name = "MENU_ID_CH0_WIDTH",
    },
    [MENU_ID_CHANNEL_1] = {
            .id = MENU_ID_CHANNEL_1,
            .name = "MENU_ID_CHANNEL_1",
    },
    [MENU_ID_CH1_ENABLE] = {
            .id = MENU_ID_CH1_ENABLE,
            .name = "MENU_ID_CH1_ENABLE",
    },
    [MENU_ID_CH1_DELAY] = {
            .id = MENU_ID_CH1_DELAY,
            .name = "MENU_ID_CH1_DELAY",
    },
    [MENU_ID_CH1_WIDTH] = {
            .id = MENU_ID_CH1_WIDTH,
            .name = "MENU_ID_CH1_WIDTH",
    },
    [MENU_ID_CHANNEL_2] = {
            .id = MENU_ID_CHANNEL_2,
            .name = "MENU_ID_CHANNEL_2",
    },
    [MENU_ID_CH2_ENABLE] = {
            .id = MENU_ID_CH2_ENABLE,
            .name = "MENU_ID_CH2_ENABLE",
    },
    [MENU_ID_CH2_DELAY] = {
            .id = MENU_ID_CH2_DELAY,
            .name = "MENU_ID_CH2_DELAY",
    },
    [MENU_ID_CH2_WIDTH] = {
            .id = MENU_ID_CH2_WIDTH,
            .name = "MENU_ID_CH2_WIDTH",
    },
    [MENU_ID_CHANNEL_3] = {
            .id = MENU_ID_CHANNEL_3,
            .name = "MENU_ID_CHANNEL_3",
    },
    [MENU_ID_CH3_ENABLE] = {
            .id = MENU_ID_CH3_ENABLE,
            .name = "MENU_ID_CH3_ENABLE",
    },
    [MENU_ID_CH3_DELAY] = {
            .id = MENU_ID_CH3_DELAY,
            .name = "MENU_ID_CH3_DELAY",
    },
    [MENU_ID_CH3_WIDTH] = {
            .id = MENU_ID_CH3_WIDTH,
            .name = "MENU_ID_CH3_WIDTH",
    },
    [MENU_ID_CHANNEL_4] = {
            .id = MENU_ID_CHANNEL_4,
            .name = "MENU_ID_CHANNEL_4",
    },
    [MENU_ID_CH4_ENABLE] = {
            .id = MENU_ID_CH4_ENABLE,
            .name = "MENU_ID_CH4_ENABLE",
    },
    [MENU_ID_CH4_DELAY] = {
            .id = MENU_ID_CH4_DELAY,
            .name = "MENU_ID_CH4_DELAY",
    },
    [MENU_ID_CH4_WIDTH] = {
            .id = MENU_ID_CH4_WIDTH,
            .name = "MENU_ID_CH4_WIDTH",
    },
    [MENU_ID_CHANNEL_5] = {
            .id = MENU_ID_CHANNEL_5,
            .name = "MENU_ID_CHANNEL_5",
    },
    [MENU_ID_CH5_ENABLE] = {
            .id = MENU_ID_CH5_ENABLE,
            .name = "MENU_ID_CH5_ENABLE",
    },
    [MENU_ID_CH5_DELAY] = {
            .id = MENU_ID_CH5_DELAY,
            .name = "MENU_ID_CH5_DELAY",
    },
    [MENU_ID_CH5_WIDTH] = {
            .id = MENU_ID_CH5_WIDTH,
            .name = "MENU_ID_CH5_WIDTH",
    },
    [MENU_ID_CHANNEL_6] = {
            .id = MENU_ID_CHANNEL_6,
            .name = "MENU_ID_CHANNEL_6",
    },
    [MENU_ID_CH6_ENABLE] = {
            .id = MENU_ID_CH6_ENABLE,
            .name = "MENU_ID_CH6_ENABLE",
    },
    [MENU_ID_CH6_DELAY] = {
            .id = MENU_ID_CH6_DELAY,
            .name = "MENU_ID_CH6_DELAY",
    },
    [MENU_ID_CH6_WIDTH] = {
            .id = MENU_ID_CH6_WIDTH,
            .name = "MENU_ID_CH6_WIDTH",
    },
    [MENU_ID_CHANNEL_7] = {
            .id = MENU_ID_CHANNEL_7,
            .name = "MENU_ID_CHANNEL_7",
    },
    [MENU_ID_CH7_ENABLE] = {
            .id = MENU_ID_CH7_ENABLE,
            .name = "MENU_ID_CH7_ENABLE",
    },
    [MENU_ID_CH7_DELAY] = {
            .id = MENU_ID_CH7_DELAY,
            .name = "MENU_ID_CH7_DELAY",
    },
    [MENU_ID_CH7_WIDTH] = {
            .id = MENU_ID_CH7_WIDTH,
            .name = "MENU_ID_CH7_WIDTH",
    },
    [MENU_ID_ANODE] = {
            .id = MENU_ID_ANODE,
            .name = "MENU_ID_ANODE",
    },
    [MENU_ID_ANODE_ENABLE] = {
            .id = MENU_ID_ANODE_ENABLE,
            .name = "MENU_ID_ANODE_ENABLE",
    },
    [MENU_ID_ANODE_PERIOD] = {
            .id = MENU_ID_ANODE_PERIOD,
            .name = "MENU_ID_ANODE_PERIOD",
    },
    [MENU_ID_ANODE_DELAY] = {
            .id = MENU_ID_ANODE_DELAY,
            .name = "MENU_ID_ANODE_DELAY",
    },
    [MENU_ID_ANODE_DURATION] = {
            .id = MENU_ID_ANODE_DURATION,
            .name = "MENU_ID_ANODE_DURATION",
    },
    [MENU_ID_ANODE_DEADTIME] = {
            .id = MENU_ID_ANODE_DEADTIME,
            .name = "MENU_ID_ANODE_DEADTIME",
    },
    [MENU_ID_ERRORS] = {
            .id = MENU_ID_ERRORS,
            .name = "MENU_ID_ERRORS",
    },
    
};

const menu_node_name_t *menu_data_get_node_names(void) {
    return s_menu_id_names;
}

