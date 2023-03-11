#pragma once

#include "compute/compute.h"

typedef enum cg_Key {
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
} cg_Key;

typedef struct cg_Canvas cg_Canvas;

typedef k_Bool(cg_start_stop_cb)(cg_Canvas* canvas, void* arg);

typedef k_Bool(cg_frame_cb)(cg_Canvas* canvas, float dt, void* arg);

typedef void(cg_key_cb)(cg_Key key, void* arg);

typedef void(cg_mouse_cb)(cg_vec2 pos, void* arg);

typedef void(cg_window_resize_cb)(cg_uvec2 size, void* arg);

typedef struct cg_Settings {
    const char* windowTitle;
    cg_uvec2 windowSize;
    k_Bool fullScreen;
    k_Bool resizable;
    cg_uvec2 canvasSize;
    void* callbackArg;
    cg_window_resize_cb* resize_cb;
    cg_start_stop_cb* start_cb;
    cg_frame_cb* frame_cb;
    cg_start_stop_cb* stop_cb;
    cg_key_cb* key_down_cb;
    cg_key_cb* key_up_cb;
    cg_mouse_cb* mouse_move_cb;
    cg_mouse_cb* mouse_scroll_cb;
} cg_Settings;

typedef struct cg_clearTool cg_clearTool;
typedef struct cg_textTool cg_textTool;

k_Result cg_start(cg_Settings settings);

cg_Canvas* cg_canvas_create(cg_uvec2 size);

cg_uvec2 cg_canvas_get_size(cg_Canvas* canvas);

cg_Buffer* cg_canvas_get_buff(cg_Canvas* canvas);

k_Result cg_canvas_clear(cg_Canvas* canvas, cg_vec4 color);

k_Result cg_canvas_printf(
    cg_Canvas* canvas,
    cg_uvec2 pos,
    uint32_t scale,
    cg_uvec2 spacing,
    cg_vec4 color,
    uint32_t tabSize,
    char* format,
    ...
);

k_Result cg_canvas_destroy(cg_Canvas* canvas);
