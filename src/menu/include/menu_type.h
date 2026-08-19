#ifndef MENU_TYPE_H
#define MENU_TYPE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define LCD_STRING_LEN 0x20
#define LCD_NUM_STRINGS 2

typedef enum {
    MENU_TREE_TYPE_NONE = 0,
    MENU_TREE_TYPE_BRANCH = 1,
    MENU_TREE_TYPE_LEAF = 2,
    MENU_TREE_TYPE_COUNT = 3
} menu_tree_type_t;

typedef enum {
    MENU_CATEGORY_NONE = 0,
    MENU_CATEGORY_STRING_FIXED = 1,
    MENU_CATEGORY_DWORD_FACTOR = 2,
    MENU_CATEGORY_DWORD_SIMPLE = 3,
    MENU_CATEGORY_CALLBACK_CALLBACK = 4,
    MENU_CATEGORY_COUNT = 5
} menu_category_t;

typedef enum {
    MENU_ID_ROOT = 0,
    
    MENU_ID_ENABLE = 1,
    
    MENU_ID_START = 2,
    
    MENU_ID_OPTIONS = 3,
    
    MENU_ID_PERIOD = 4,
    
    MENU_ID_LEG_A = 5,
    
    MENU_ID_LEG_A_WIDTH = 6,
    
    MENU_ID_LEG_B = 7,
    
    MENU_ID_LEG_B_ENABLE = 8,
    
    MENU_ID_LEG_B_DELAY = 9,
    
    MENU_ID_LEG_B_WIDTH = 10,
    
    MENU_ID_CHANNEL_0 = 11,
    
    MENU_ID_CH0_ENABLE = 12,
    
    MENU_ID_CH0_DELAY = 13,
    
    MENU_ID_CH0_WIDTH = 14,
    
    MENU_ID_CHANNEL_1 = 15,
    
    MENU_ID_CH1_ENABLE = 16,
    
    MENU_ID_CH1_DELAY = 17,
    
    MENU_ID_CH1_WIDTH = 18,
    
    MENU_ID_CHANNEL_2 = 19,
    
    MENU_ID_CH2_ENABLE = 20,
    
    MENU_ID_CH2_DELAY = 21,
    
    MENU_ID_CH2_WIDTH = 22,
    
    MENU_ID_CHANNEL_3 = 23,
    
    MENU_ID_CH3_ENABLE = 24,
    
    MENU_ID_CH3_DELAY = 25,
    
    MENU_ID_CH3_WIDTH = 26,
    
    MENU_ID_CHANNEL_4 = 27,
    
    MENU_ID_CH4_ENABLE = 28,
    
    MENU_ID_CH4_DELAY = 29,
    
    MENU_ID_CH4_WIDTH = 30,
    
    MENU_ID_CHANNEL_5 = 31,
    
    MENU_ID_CH5_ENABLE = 32,
    
    MENU_ID_CH5_DELAY = 33,
    
    MENU_ID_CH5_WIDTH = 34,
    
    MENU_ID_CHANNEL_6 = 35,
    
    MENU_ID_CH6_ENABLE = 36,
    
    MENU_ID_CH6_DELAY = 37,
    
    MENU_ID_CH6_WIDTH = 38,
    
    MENU_ID_CHANNEL_7 = 39,
    
    MENU_ID_CH7_ENABLE = 40,
    
    MENU_ID_CH7_DELAY = 41,
    
    MENU_ID_CH7_WIDTH = 42,
    
    MENU_ID_ANODE = 43,
    
    MENU_ID_ANODE_ENABLE = 44,
    
    MENU_ID_ANODE_PERIOD = 45,
    
    MENU_ID_ANODE_DELAY = 46,
    
    MENU_ID_ANODE_DURATION = 47,
    
    MENU_ID_ANODE_DEADTIME = 48,
    
    MENU_ID_ERRORS = 49,
    
    MENU_ID_COUNT = 50
} menu_id_t;

typedef enum {
    MENU_EVENT_NONE = 0,
    MENU_EVENT_CHANGE_VALUE = 1,
    MENU_EVENT_FOCUSED = 2,
    MENU_EVENT_UNFOCUSED = 3,
    MENU_EVENT_START_EDIT = 4,
    MENU_EVENT_STOP_EDIT = 5,
    MENU_EVENT_COUNT = 6
} menu_event_t;

typedef enum {
    MENU_STATE_NONE = 0,
    MENU_STATE_NAVIGATION = 1,
    MENU_STATE_EDIT = 2,
    MENU_STATE_COUNT = 3
} menu_state_t;

// Базовые структуры (только forward declarations)
typedef struct menu_context menu_context_t;
typedef struct menu_node menu_node_t;
typedef struct menu_node_config menu_node_config_t;
typedef struct menu_node_value menu_node_value_t;
typedef struct menu_node_name menu_node_name_t;

#endif /* MENU_TYPE_H */