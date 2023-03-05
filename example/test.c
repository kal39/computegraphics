#include <microcanvas.h>
#include <stdio.h>

static char* renderProgSrc = //
    "#version 460\n"
    "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer ssbo0 {\n"
    "    vec3 floatData[];\n"
    "};\n"
    "uniform float maxIter;\n"
    "uniform vec2 center;\n"
    "uniform vec2 zoom;\n"
    "void main(void) {\n"
    "    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);\n"
    "    ivec2 size = ivec2(gl_NumWorkGroups.xy);\n"
    "    vec2 screenPos = vec2(pos) / vec2(size) - 0.5;\n"
    "    vec2 z0 = center + screenPos / zoom;\n"
    "    vec2 z = vec2(0.0, 0.0);\n"
    "    vec2 z2 = vec2(0.0, 0.0);\n"
    "    int i = 0;\n"
    "    for(; i < maxIter && z2.x + z2.y <= 4; i++) {\n"
    "        z = vec2(z2.x - z2.y + z0.x, 2 * z.x * z.y + z0.y);\n"
    "        z2 = vec2(z.x * z.x, z.y * z.y);\n"
    "    }\n"
    "    float color = float(i) / float(maxIter);\n"
    "    floatData[(pos.y * size.x + pos.x)] = vec3(color, \n"
    "0.5 * color, 0.5 * color);\n"
    "}\n";

typedef struct State {
    mc_Program* renderProg;
    mcv_canvasClearTool* clearTool;
    mcv_textDrawTool* textTool;
} State;

mc_Bool start(mcv_Canvas canvas, State* state) {
    printf("start\n");

    int maxErrLen = 2048;
    char error[maxErrLen];

    state->renderProg
        = mc_program_create_from_string(renderProgSrc, maxErrLen, error);
    if (state->renderProg == NULL) {
        printf("error: %s\n", error);
        return MC_FALSE;
    }

    state->clearTool = mcv_canvas_clear_tool_create();
    mcv_canvas_clear_tool_set_color(
        state->clearTool,
        (mc_vec4){0.5, 0.5, 0.0, 1.0}
    );

    state->textTool = mcv_text_draw_tool_create();
    mcv_text_draw_tool_set_scale(state->textTool, 3);

    return MC_TRUE;
}

mc_Bool frame(mcv_Canvas canvas, float dt, State* state) {
    mc_program_set_float(state->renderProg, "maxIter", 500);
    mc_program_set_vec2(
        state->renderProg,
        "center",
        (mc_vec2){-0.7615, -0.08459}
    );
    mc_program_set_vec2(state->renderProg, "zoom", (mc_vec2){1000, 1000});

    mc_program_dispatch(
        state->renderProg,
        (mc_ivec3){canvas.size.x, canvas.size.y, 1},
        1,
        (mc_Buffer*[]){canvas.buff}
    );

    char msg[100];
    sprintf(msg, "%d fps", (int)(1.0 / dt));

    // mcv_canvas_clear_tool_clear(state->clearTool, canvas);
    mcv_text_draw_tool_draw(state->textTool, canvas, msg, (mc_uvec2){20, 20});

    return MC_TRUE;
}

mc_Bool stop(mcv_Canvas canvas, State* state) {
    printf("stop\n");

    mc_program_destroy(state->renderProg);
    mcv_canvas_clear_tool_destroy(state->clearTool);
    mcv_text_draw_tool_destroy(state->textTool);

    return MC_TRUE;
}

int main(void) {
    State state;

    mcv_Settings settings = (mcv_Settings){
        .windowTitle = "Mandelbrot Test",
        .windowSize = (mc_uvec2){1000, 800},
        .canvasSize = (mc_uvec2){1000, 800},
        .callbackArg = &state,
        .start_cb_fn = (mcv_start_stop_callback*)start,
        .frame_cb_fn = (mcv_frame_callback*)frame,
        .stop_cb_fn = (mcv_start_stop_callback*)stop,
    };

    mc_Result res = mcv_start(settings);
    if (!res.ok) {
        mc_result_pretty_print(res);
        return 1;
    }

    return 0;
}