#include "menu_data_tree.h"
#include "menu_tree.h"

static const menu_node_t s_menu_tree[] = {
    [MENU_ID_ROOT] = {
        .id = MENU_ID_ROOT,
        .title = "root",
        .parent = MENU_ID_COUNT,
        .child = MENU_ID_ENABLE,
        .prev = MENU_ID_COUNT,
        .next = MENU_ID_COUNT,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_ENABLE] = {
        .id = MENU_ID_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_ROOT,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_ERRORS,
        .next = MENU_ID_START,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_START] = {
        .id = MENU_ID_START,
        .title = "Start",
        .parent = MENU_ID_ROOT,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_ENABLE,
        .next = MENU_ID_OPTIONS,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_OPTIONS] = {
        .id = MENU_ID_OPTIONS,
        .title = "Options",
        .parent = MENU_ID_ROOT,
        .child = MENU_ID_PERIOD,
        .prev = MENU_ID_START,
        .next = MENU_ID_ERRORS,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_PERIOD] = {
        .id = MENU_ID_PERIOD,
        .title = "Period",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_COUNT,
        .next = MENU_ID_LEG_A,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_LEG_A] = {
        .id = MENU_ID_LEG_A,
        .title = "Leg A",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_LEG_A_WIDTH,
        .prev = MENU_ID_PERIOD,
        .next = MENU_ID_LEG_B,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_LEG_A_WIDTH] = {
        .id = MENU_ID_LEG_A_WIDTH,
        .title = "Width",
        .parent = MENU_ID_LEG_A,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_COUNT,
        .next = MENU_ID_COUNT,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_LEG_B] = {
        .id = MENU_ID_LEG_B,
        .title = "Leg B",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_LEG_B_ENABLE,
        .prev = MENU_ID_LEG_A,
        .next = MENU_ID_CHANNEL_0,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_LEG_B_ENABLE] = {
        .id = MENU_ID_LEG_B_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_LEG_B,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_LEG_B_WIDTH,
        .next = MENU_ID_LEG_B_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_LEG_B_DELAY] = {
        .id = MENU_ID_LEG_B_DELAY,
        .title = "Delay",
        .parent = MENU_ID_LEG_B,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_LEG_B_ENABLE,
        .next = MENU_ID_LEG_B_WIDTH,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_LEG_B_WIDTH] = {
        .id = MENU_ID_LEG_B_WIDTH,
        .title = "Width",
        .parent = MENU_ID_LEG_B,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_LEG_B_DELAY,
        .next = MENU_ID_LEG_B_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CHANNEL_0] = {
        .id = MENU_ID_CHANNEL_0,
        .title = "Channel 0",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_CH0_ENABLE,
        .prev = MENU_ID_LEG_B,
        .next = MENU_ID_CHANNEL_1,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_CH0_ENABLE] = {
        .id = MENU_ID_CH0_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_CHANNEL_0,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH0_WIDTH,
        .next = MENU_ID_CH0_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH0_DELAY] = {
        .id = MENU_ID_CH0_DELAY,
        .title = "Delay",
        .parent = MENU_ID_CHANNEL_0,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH0_ENABLE,
        .next = MENU_ID_CH0_WIDTH,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH0_WIDTH] = {
        .id = MENU_ID_CH0_WIDTH,
        .title = "Width",
        .parent = MENU_ID_CHANNEL_0,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH0_DELAY,
        .next = MENU_ID_CH0_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CHANNEL_1] = {
        .id = MENU_ID_CHANNEL_1,
        .title = "Channel 1",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_CH1_ENABLE,
        .prev = MENU_ID_CHANNEL_0,
        .next = MENU_ID_CHANNEL_2,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_CH1_ENABLE] = {
        .id = MENU_ID_CH1_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_CHANNEL_1,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH1_WIDTH,
        .next = MENU_ID_CH1_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH1_DELAY] = {
        .id = MENU_ID_CH1_DELAY,
        .title = "Delay",
        .parent = MENU_ID_CHANNEL_1,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH1_ENABLE,
        .next = MENU_ID_CH1_WIDTH,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH1_WIDTH] = {
        .id = MENU_ID_CH1_WIDTH,
        .title = "Width",
        .parent = MENU_ID_CHANNEL_1,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH1_DELAY,
        .next = MENU_ID_CH1_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CHANNEL_2] = {
        .id = MENU_ID_CHANNEL_2,
        .title = "Channel 2",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_CH2_ENABLE,
        .prev = MENU_ID_CHANNEL_1,
        .next = MENU_ID_CHANNEL_3,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_CH2_ENABLE] = {
        .id = MENU_ID_CH2_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_CHANNEL_2,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH2_WIDTH,
        .next = MENU_ID_CH2_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH2_DELAY] = {
        .id = MENU_ID_CH2_DELAY,
        .title = "Delay",
        .parent = MENU_ID_CHANNEL_2,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH2_ENABLE,
        .next = MENU_ID_CH2_WIDTH,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH2_WIDTH] = {
        .id = MENU_ID_CH2_WIDTH,
        .title = "Width",
        .parent = MENU_ID_CHANNEL_2,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH2_DELAY,
        .next = MENU_ID_CH2_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CHANNEL_3] = {
        .id = MENU_ID_CHANNEL_3,
        .title = "Channel 3",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_CH3_ENABLE,
        .prev = MENU_ID_CHANNEL_2,
        .next = MENU_ID_CHANNEL_4,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_CH3_ENABLE] = {
        .id = MENU_ID_CH3_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_CHANNEL_3,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH3_WIDTH,
        .next = MENU_ID_CH3_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH3_DELAY] = {
        .id = MENU_ID_CH3_DELAY,
        .title = "Delay",
        .parent = MENU_ID_CHANNEL_3,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH3_ENABLE,
        .next = MENU_ID_CH3_WIDTH,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH3_WIDTH] = {
        .id = MENU_ID_CH3_WIDTH,
        .title = "Width",
        .parent = MENU_ID_CHANNEL_3,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH3_DELAY,
        .next = MENU_ID_CH3_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CHANNEL_4] = {
        .id = MENU_ID_CHANNEL_4,
        .title = "Channel 4",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_CH4_ENABLE,
        .prev = MENU_ID_CHANNEL_3,
        .next = MENU_ID_CHANNEL_5,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_CH4_ENABLE] = {
        .id = MENU_ID_CH4_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_CHANNEL_4,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH4_WIDTH,
        .next = MENU_ID_CH4_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH4_DELAY] = {
        .id = MENU_ID_CH4_DELAY,
        .title = "Delay",
        .parent = MENU_ID_CHANNEL_4,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH4_ENABLE,
        .next = MENU_ID_CH4_WIDTH,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH4_WIDTH] = {
        .id = MENU_ID_CH4_WIDTH,
        .title = "Width",
        .parent = MENU_ID_CHANNEL_4,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH4_DELAY,
        .next = MENU_ID_CH4_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CHANNEL_5] = {
        .id = MENU_ID_CHANNEL_5,
        .title = "Channel 5",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_CH5_ENABLE,
        .prev = MENU_ID_CHANNEL_4,
        .next = MENU_ID_CHANNEL_6,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_CH5_ENABLE] = {
        .id = MENU_ID_CH5_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_CHANNEL_5,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH5_WIDTH,
        .next = MENU_ID_CH5_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH5_DELAY] = {
        .id = MENU_ID_CH5_DELAY,
        .title = "Delay",
        .parent = MENU_ID_CHANNEL_5,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH5_ENABLE,
        .next = MENU_ID_CH5_WIDTH,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH5_WIDTH] = {
        .id = MENU_ID_CH5_WIDTH,
        .title = "Width",
        .parent = MENU_ID_CHANNEL_5,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH5_DELAY,
        .next = MENU_ID_CH5_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CHANNEL_6] = {
        .id = MENU_ID_CHANNEL_6,
        .title = "Channel 6",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_CH6_ENABLE,
        .prev = MENU_ID_CHANNEL_5,
        .next = MENU_ID_CHANNEL_7,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_CH6_ENABLE] = {
        .id = MENU_ID_CH6_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_CHANNEL_6,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH6_WIDTH,
        .next = MENU_ID_CH6_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH6_DELAY] = {
        .id = MENU_ID_CH6_DELAY,
        .title = "Delay",
        .parent = MENU_ID_CHANNEL_6,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH6_ENABLE,
        .next = MENU_ID_CH6_WIDTH,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH6_WIDTH] = {
        .id = MENU_ID_CH6_WIDTH,
        .title = "Width",
        .parent = MENU_ID_CHANNEL_6,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH6_DELAY,
        .next = MENU_ID_CH6_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CHANNEL_7] = {
        .id = MENU_ID_CHANNEL_7,
        .title = "Channel 7",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_CH7_ENABLE,
        .prev = MENU_ID_CHANNEL_6,
        .next = MENU_ID_ANODE,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_CH7_ENABLE] = {
        .id = MENU_ID_CH7_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_CHANNEL_7,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH7_WIDTH,
        .next = MENU_ID_CH7_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH7_DELAY] = {
        .id = MENU_ID_CH7_DELAY,
        .title = "Delay",
        .parent = MENU_ID_CHANNEL_7,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH7_ENABLE,
        .next = MENU_ID_CH7_WIDTH,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_CH7_WIDTH] = {
        .id = MENU_ID_CH7_WIDTH,
        .title = "Width",
        .parent = MENU_ID_CHANNEL_7,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_CH7_DELAY,
        .next = MENU_ID_CH7_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_ANODE] = {
        .id = MENU_ID_ANODE,
        .title = "Anode",
        .parent = MENU_ID_OPTIONS,
        .child = MENU_ID_ANODE_ENABLE,
        .prev = MENU_ID_CHANNEL_7,
        .next = MENU_ID_COUNT,
        .type = MENU_TREE_TYPE_BRANCH
    },
    [MENU_ID_ANODE_ENABLE] = {
        .id = MENU_ID_ANODE_ENABLE,
        .title = "Enable",
        .parent = MENU_ID_ANODE,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_ANODE_DEADTIME,
        .next = MENU_ID_ANODE_PERIOD,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_ANODE_PERIOD] = {
        .id = MENU_ID_ANODE_PERIOD,
        .title = "Period",
        .parent = MENU_ID_ANODE,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_ANODE_ENABLE,
        .next = MENU_ID_ANODE_DELAY,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_ANODE_DELAY] = {
        .id = MENU_ID_ANODE_DELAY,
        .title = "Delay",
        .parent = MENU_ID_ANODE,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_ANODE_PERIOD,
        .next = MENU_ID_ANODE_DURATION,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_ANODE_DURATION] = {
        .id = MENU_ID_ANODE_DURATION,
        .title = "Duration",
        .parent = MENU_ID_ANODE,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_ANODE_DELAY,
        .next = MENU_ID_ANODE_DEADTIME,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_ANODE_DEADTIME] = {
        .id = MENU_ID_ANODE_DEADTIME,
        .title = "Deadtime",
        .parent = MENU_ID_ANODE,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_ANODE_DURATION,
        .next = MENU_ID_ANODE_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    [MENU_ID_ERRORS] = {
        .id = MENU_ID_ERRORS,
        .title = "Errors",
        .parent = MENU_ID_ROOT,
        .child = MENU_ID_COUNT,
        .prev = MENU_ID_OPTIONS,
        .next = MENU_ID_ENABLE,
        .type = MENU_TREE_TYPE_LEAF
    },
    
};

const menu_node_t *menu_data_get_tree(void) {
    return s_menu_tree;
}

menu_id_t menu_data_get_first_id(void) {
    return MENU_ID_ENABLE;
}
