#pragma once

#include <microcompute.h>

typedef enum mcv_Key {
    MCV_KEY_UNKNOWN = -1,
    MCV_KEY_SPACE = 32,
    MCV_KEY_APOSTROPHE = 39,
    MCV_KEY_COMMA = 44,
    MCV_KEY_MINUS = 45,
    MCV_KEY_PERIOD = 46,
    MCV_KEY_SLASH = 47,
    MCV_KEY_0 = 48,
    MCV_KEY_1 = 49,
    MCV_KEY_2 = 50,
    MCV_KEY_3 = 51,
    MCV_KEY_4 = 52,
    MCV_KEY_5 = 53,
    MCV_KEY_6 = 54,
    MCV_KEY_7 = 55,
    MCV_KEY_8 = 56,
    MCV_KEY_9 = 57,
    MCV_KEY_SEMICOLON = 59,
    MCV_KEY_EQUAL = 61,
    MCV_KEY_A = 65,
    MCV_KEY_B = 66,
    MCV_KEY_C = 67,
    MCV_KEY_D = 68,
    MCV_KEY_E = 69,
    MCV_KEY_F = 70,
    MCV_KEY_G = 71,
    MCV_KEY_H = 72,
    MCV_KEY_I = 73,
    MCV_KEY_J = 74,
    MCV_KEY_K = 75,
    MCV_KEY_L = 76,
    MCV_KEY_M = 77,
    MCV_KEY_N = 78,
    MCV_KEY_O = 79,
    MCV_KEY_P = 80,
    MCV_KEY_Q = 81,
    MCV_KEY_R = 82,
    MCV_KEY_S = 83,
    MCV_KEY_T = 84,
    MCV_KEY_U = 85,
    MCV_KEY_V = 86,
    MCV_KEY_W = 87,
    MCV_KEY_X = 88,
    MCV_KEY_Y = 89,
    MCV_KEY_Z = 90,
    MCV_KEY_LEFT_BRACKET = 91,
    MCV_KEY_BACKSLASH = 92,
    MCV_KEY_RIGHT_BRACKET = 93,
    MCV_KEY_GRAVE_ACCENT = 96,
    MCV_KEY_WORLD_1 = 161,
    MCV_KEY_WORLD_2 = 162,
    MCV_KEY_ESCAPE = 256,
    MCV_KEY_ENTER = 257,
    MCV_KEY_TAB = 258,
    MCV_KEY_BACKSPACE = 259,
    MCV_KEY_INSERT = 260,
    MCV_KEY_DELETE = 261,
    MCV_KEY_RIGHT = 262,
    MCV_KEY_LEFT = 263,
    MCV_KEY_DOWN = 264,
    MCV_KEY_UP = 265,
    MCV_KEY_PAGE_UP = 266,
    MCV_KEY_PAGE_DOWN = 267,
    MCV_KEY_HOME = 268,
    MCV_KEY_END = 269,
    MCV_KEY_CAPS_LOCK = 280,
    MCV_KEY_SCROLL_LOCK = 281,
    MCV_KEY_NUM_LOCK = 282,
    MCV_KEY_PRINT_SCREEN = 283,
    MCV_KEY_PAUSE = 284,
    MCV_KEY_F1 = 290,
    MCV_KEY_F2 = 291,
    MCV_KEY_F3 = 292,
    MCV_KEY_F4 = 293,
    MCV_KEY_F5 = 294,
    MCV_KEY_F6 = 295,
    MCV_KEY_F7 = 296,
    MCV_KEY_F8 = 297,
    MCV_KEY_F9 = 298,
    MCV_KEY_F10 = 299,
    MCV_KEY_F11 = 300,
    MCV_KEY_F12 = 301,
    MCV_KEY_F13 = 302,
    MCV_KEY_F14 = 303,
    MCV_KEY_F15 = 304,
    MCV_KEY_F16 = 305,
    MCV_KEY_F17 = 306,
    MCV_KEY_F18 = 307,
    MCV_KEY_F19 = 308,
    MCV_KEY_F20 = 309,
    MCV_KEY_F21 = 310,
    MCV_KEY_F22 = 311,
    MCV_KEY_F23 = 312,
    MCV_KEY_F24 = 313,
    MCV_KEY_F25 = 314,
    MCV_KEY_KEYPAD_0 = 320,
    MCV_KEY_KEYPAD_1 = 321,
    MCV_KEY_KEYPAD_2 = 322,
    MCV_KEY_KEYPAD_3 = 323,
    MCV_KEY_KEYPAD_4 = 324,
    MCV_KEY_KEYPAD_5 = 325,
    MCV_KEY_KEYPAD_6 = 326,
    MCV_KEY_KEYPAD_7 = 327,
    MCV_KEY_KEYPAD_8 = 328,
    MCV_KEY_KEYPAD_9 = 329,
    MCV_KEY_KEYPAD_DECIMAL = 330,
    MCV_KEY_KEYPAD_DIVIDE = 331,
    MCV_KEY_KEYPAD_MULTIPLY = 332,
    MCV_KEY_KEYPAD_SUBTRACT = 333,
    MCV_KEY_KEYPAD_ADD = 334,
    MCV_KEY_KEYPAD_ENTER = 335,
    MCV_KEY_KEYPAD_EQUAL = 336,
    MCV_KEY_LEFT_SHIFT = 340,
    MCV_KEY_LEFT_CONTROL = 341,
    MCV_KEY_LEFT_ALT = 342,
    MCV_KEY_LEFT_SUPER = 343,
    MCV_KEY_RIGHT_SHIFT = 344,
    MCV_KEY_RIGHT_CONTROL = 345,
    MCV_KEY_RIGHT_ALT = 346,
    MCV_KEY_RIGHT_SUPER = 347,
    MCV_KEY_MENU = 348,
    MCV_KEY_MOUSE_1 = 400,
    MCV_KEY_MOUSE_2 = 401,
    MCV_KEY_MOUSE_3 = 402,
    MCV_KEY_MOUSE_4 = 403,
    MCV_KEY_MOUSE_5 = 404,
    MCV_KEY_MOUSE_6 = 405,
    MCV_KEY_MOUSE_7 = 406,
    MCV_KEY_MOUSE_8 = 407,
} mcv_Key;

typedef struct mcv_Canvas {
    mc_uvec2 size;
    mc_Buffer* buff;
} mcv_Canvas;

typedef mc_Bool(mcv_start_stop_cb)(mcv_Canvas canvas, void* arg);

typedef mc_Bool(mcv_frame_cb)(mcv_Canvas canvas, float dt, void* arg);

typedef void(mcv_key_cb)(mcv_Key key, void* arg);

typedef void(mcv_mouse_cb)(mc_vec2 pos, void* arg);

typedef struct mcv_Settings {
    const char* windowTitle;
    mc_uvec2 windowSize;
    mc_uvec2 canvasSize;
    void* callbackArg;
    mcv_start_stop_cb* start_cb;
    mcv_frame_cb* frame_cb;
    mcv_start_stop_cb* stop_cb;
    mcv_key_cb* key_down_cb;
    mcv_key_cb* key_up_cb;
    mcv_mouse_cb* mouse_move_cb;
    mcv_mouse_cb* mouse_scroll_cb;
} mcv_Settings;

typedef struct mcv_clearTool mcv_clearTool;
typedef struct mcv_textTool mcv_textTool;

mc_Result mcv_start(mcv_Settings settings);

mcv_clearTool* mcv_clear_tool_create();

mc_Result mcv_clear_tool_destroy(mcv_clearTool* clearTool);

mc_Result mcv_clear_tool_set_color(mcv_clearTool* clearTool, mc_vec4 color);

mc_Result mcv_clear_tool_clear(mcv_clearTool* clearTool, mcv_Canvas canvas);

mcv_textTool* mcv_text_tool_create();

mc_Result mcv_text_tool_destroy(mcv_textTool* textTool);

mc_Result mcv_text_tool_set_scale(mcv_textTool* textTool, uint32_t scale);

mc_Result mcv_text_tool_set_spacing(mcv_textTool* textTool, mc_uvec2 spacing);

mc_Result mcv_text_tool_set_color(mcv_textTool* textTool, mc_vec4 color);

mc_Result mcv_text_tool_set_tab_size(mcv_textTool* textTool, uint32_t size);

mc_Result mvc_text_tool_printf(
    mcv_textTool* textTool,
    mcv_Canvas canvas,
    mc_uvec2 pos,
    char* format,
    ...
);
