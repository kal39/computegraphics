#include <microcanvas.h>
#include <stdio.h>

const char* text = //
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod\n"
    "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim\n"
    "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea\n"
    "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate\n"
    "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint\n"
    "occaecat cupidatat non proident, sunt in culpa qui officia deserunt\n"
    "mollit anim id est laborum.\n";

typedef struct State {
    mcv_clearTool* clearTool;
    mcv_textTool* textTool;
} State;

mc_Bool start(mcv_Canvas cv, State* s) {
    s->clearTool = mcv_clear_tool_create();
    s->textTool = mcv_text_tool_create();
    mcv_text_tool_set_scale(s->textTool, 2);
    mcv_text_tool_set_spacing(s->textTool, (mc_uvec2){0, 0});
    return MC_TRUE;
}

mc_Bool frame(mcv_Canvas cv, float dt, State* s) {
    mcv_clear_tool_clear(s->clearTool, cv);
    mvc_text_tool_printf(
        s->textTool,
        cv,
        (mc_uvec2){20, 20},
        "%d fps",
        (int)(1.0 / dt)
    );

    mvc_text_tool_printf(s->textTool, cv, (mc_uvec2){20, 40}, "%s", text);
    mvc_text_tool_printf(s->textTool, cv, (mc_uvec2){20, 240}, "%s", text);
    mvc_text_tool_printf(s->textTool, cv, (mc_uvec2){20, 440}, "%s", text);
    mvc_text_tool_printf(s->textTool, cv, (mc_uvec2){20, 640}, "%s", text);

    return MC_TRUE;
}

mc_Bool stop(mcv_Canvas cv, State* s) {
    mcv_clear_tool_destroy(s->clearTool);
    mcv_text_tool_destroy(s->textTool);
    return MC_TRUE;
}

int main(void) {
    mc_Result res = mcv_start((mcv_Settings){
        .windowTitle = "Text Stress Test",
        .windowSize = (mc_uvec2){1000, 800},
        .canvasSize = (mc_uvec2){1000, 800},
        .callbackArg = &(State){},
        .start_cb = (mcv_start_stop_cb*)start,
        .frame_cb = (mcv_frame_cb*)frame,
        .stop_cb = (mcv_start_stop_cb*)stop,
    });
    if (!res.ok) mc_result_pretty_print(res);
}