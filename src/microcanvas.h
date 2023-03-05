#pragma once

#include <microcompute.h>

typedef struct mcv_Canvas {
    mc_uvec2 size;
    mc_Buffer* buff;
} mcv_Canvas;

typedef mc_Bool(mcv_start_stop_callback)( //
    mcv_Canvas canvas,
    void* arg
);

typedef mc_Bool(mcv_frame_callback)( //
    mcv_Canvas canvas,
    float dt,
    void* arg
);

typedef struct mcv_Settings {
    const char* windowTitle;
    mc_uvec2 windowSize;
    mc_uvec2 canvasSize;
    void* callbackArg;
    mcv_start_stop_callback* start_cb_fn;
    mcv_frame_callback* frame_cb_fn;
    mcv_start_stop_callback* stop_cb_fn;
} mcv_Settings;

typedef struct mcv_canvasClearTool mcv_canvasClearTool;

mc_Result mcv_start(mcv_Settings settings);

mcv_canvasClearTool* mcv_canvas_clear_tool_create();

mc_Result mcv_canvas_clear_tool_set_color(
    mcv_canvasClearTool* clearTool,
    mc_vec4 clearColor
);

mc_Result mcv_canvas_clear_tool_clear(
    mcv_canvasClearTool* clearTool,
    mcv_Canvas canvas
);

mc_Result mcv_canvas_clear_tool_destroy(mcv_canvasClearTool* clearTool);