#pragma once

#include <microcompute.h>

typedef struct mf_Canvas {
    mc_uvec2 size;
    mc_Buffer* buff;
} mf_Canvas;

typedef mc_Bool(mf_start_stop_callback)( //
    mf_Canvas canvas,
    void* arg
);

typedef mc_Bool(mf_frame_callback)( //
    mf_Canvas canvas,
    float dt,
    void* arg
);

typedef struct mf_Settings {
    const char* windowTitle;
    mc_uvec2 windowSize;
    mc_uvec2 canvasSize;
    void* callbackArg;
    mf_start_stop_callback* start_cb_fn;
    mf_frame_callback* frame_cb_fn;
    mf_start_stop_callback* stop_cb_fn;
} mf_Settings;

typedef struct mf_canvasClearTool mf_canvasClearTool;

mc_Result mf_start(mf_Settings settings);

mf_canvasClearTool* mf_canvas_clear_tool_create();

mc_Result mf_canvas_clear_tool_set_color(
    mf_canvasClearTool* clearTool,
    mc_vec4 clearColor
);

mc_Result mf_canvas_clear_tool_clear(
    mf_canvasClearTool* clearTool,
    mf_Canvas canvas
);

mc_Result mf_canvas_clear_tool_destroy(mf_canvasClearTool* clearTool);