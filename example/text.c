#include <microcanvas.h>
#include <stdio.h>

static char* text1 = //
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod\n"
    "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim\n"
    "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea\n"
    "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate\n"
    "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint\n"
    "occaecat cupidatat non proident, sunt in culpa qui officia deserunt\n"
    "mollit anim id est laborum.\n";

static char* text2 = //
    "#version 430\n"
    "layout(local_size_x = 1, local_size_y = 1) in;\n\n"
    "layout(std430, binding = 0) restrict coherent writeonly buffer ssbo0 {\n"
    "   vec4 cv[];\n"
    "};\n\n"
    "uniform ivec2 charData;\n"
    "uniform uvec2 cvSize;\n"
    "uniform uvec2 charPos;\n"
    "uniform vec4  charColor;\n"
    "uniform uint  charScale;\n\n"
    "void main() {\n"
    "   uvec2 fPos = gl_GlobalInvocationID.xy / charScale;\n"
    "   uvec2 pPos = gl_GlobalInvocationID.xy + charPos;\n\n"
    "   int draw = fPos.y < 5\n"
    "       ? (charData.y >> (29 - (fPos.y    ) * 6 - fPos.x)) & 1\n"
    "       : (charData.x >> (29 - (fPos.y - 5) * 6 - fPos.x)) & 1;\n\n"
    "   if(bool(draw)) cv[pPos.y * cvSize.x + pPos.x] = charColor;\n"
    "}\n";

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
    mvc_text_tool_printf(s->textTool, cv, (mc_uvec2){20, 20}, "%f fps", 1 / dt);
    mvc_text_tool_printf(s->textTool, cv, (mc_uvec2){20, 50}, "%s", text1);
    mvc_text_tool_printf(s->textTool, cv, (mc_uvec2){20, 250}, "%s", text2);
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