#include "_microcanvas.h"

static char* code = //
    "#version 430\n"
    "layout(local_size_x = 1, local_size_y = 1) in;\n"
    "uniform vec4 clearColor;\n"
    "layout(std430, binding = 0) buffer ssbo0 {\n"
    "    vec4 cv[];\n"
    "};\n"
    "void main() {\n"
    "   uvec2 pos = gl_GlobalInvocationID.xy;\n"
    "   uvec2 size = gl_NumWorkGroups.xy;\n"
    "   cv[pos.y * size.x + pos.x] = clearColor;\n"
    "}\n";

mcv_canvasClearTool* mcv_canvas_clear_tool_create() {
    mcv_canvasClearTool* clearTool = malloc(sizeof(*clearTool));
    clearTool->program = mc_program_create_from_string(code, 0, NULL);

    if (clearTool->program == NULL) {
        free(clearTool);
        return NULL;
    }

    mcv_canvas_clear_tool_set_color(clearTool, (mc_vec4){0.0, 0.0, 0.0, 1.0});
    return clearTool;
}

mc_Result mcv_canvas_clear_tool_set_color(
    mcv_canvasClearTool* clearTool,
    mc_vec4 clearColor
) {
    mc_program_set_vec4(clearTool->program, "clearColor", clearColor);
    return OK;
}

mc_Result mcv_canvas_clear_tool_clear(
    mcv_canvasClearTool* clearTool,
    mcv_Canvas canvas
) {
    return mc_program_dispatch(
        clearTool->program,
        (mc_ivec3){canvas.size.x, canvas.size.y, 1},
        1,
        (mc_Buffer*[]){canvas.buff}
    );
}

mc_Result mcv_canvas_clear_tool_destroy(mcv_canvasClearTool* clearTool) {
    mc_program_destroy(clearTool->program);
    free(clearTool);
    return OK;
}