#pragma once

#include <microcompute.h>

typedef struct mcv_Canvas {
    mc_uvec2 size;
    mc_Buffer* buff;
} mcv_Canvas;

typedef mc_Bool(mcv_start_stop_cb)( //
    mcv_Canvas canvas,
    void* arg
);

typedef mc_Bool(mcv_frame_cb)( //
    mcv_Canvas canvas,
    float dt,
    void* arg
);

typedef struct mcv_Settings {
    const char* windowTitle;
    mc_uvec2 windowSize;
    mc_uvec2 canvasSize;
    void* callbackArg;
    mcv_start_stop_cb* start_cb;
    mcv_frame_cb* frame_cb;
    mcv_start_stop_cb* stop_cb;
} mcv_Settings;

typedef struct mcv_clearTool mcv_clearTool;
typedef struct mcv_textTool mcv_textTool;

mc_Result mcv_start(mcv_Settings settings);

mcv_clearTool* mcv_clear_tool_create();

mc_Result mcv_clear_tool_set_color(mcv_clearTool* clearTool, mc_vec4 color);

mc_Result mcv_clear_tool_clear(mcv_clearTool* clearTool, mcv_Canvas canvas);

mc_Result mcv_clear_tool_destroy(mcv_clearTool* clearTool);

mcv_textTool* mcv_text_tool_create();

mc_Result mcv_text_tool_set_scale(mcv_textTool* textTool, uint32_t scale);

mc_Result mcv_text_tool_set_spacing(mcv_textTool* textTool, uint32_t spacing);

mc_Result mcv_text_tool_set_color(mcv_textTool* textTool, mc_vec4 color);

mc_Result mcv_text_tool_draw(
    mcv_textTool* textTool,
    mcv_Canvas canvas,
    char* text,
    mc_uvec2 pos
);

mc_Result mcv_text_tool_destroy(mcv_textTool* textTool);