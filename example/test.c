#include <math.h>
#include <microcanvas.h>
#include <stdio.h>

#define MOVE_SPEED 0.5
#define ZOOM_SPEED 2

static char* progSrc = //
    "#version 460\n"
    "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer ssbo0 {\n"
    "    vec3 floatData[];\n"
    "};\n"
    "uniform float maxIter;\n"
    "uniform vec2 center;\n"
    "uniform float zoom;\n"
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
    mc_Program* prog;
    mcv_clearTool* clearTool;
    mcv_textTool* textTool;

    mc_vec2 cameraPos;
    mc_vec2 cameraPosDelta;
    float cameraZoom;
    float cameraZoomDelta;
} State;

mc_Bool start(mcv_Canvas canvas, State* state) {
    int maxErrLen = 2048;
    char error[maxErrLen];

    state->prog = mc_program_from_string(progSrc, maxErrLen, error);
    if (state->prog == NULL) {
        printf("error: %s\n", error);
        return MC_FALSE;
    }

    state->clearTool = mcv_clear_tool_create();
    state->textTool = mcv_text_tool_create();
    state->cameraPos = (mc_vec2){-0.7615, -0.08459};
    state->cameraPosDelta = (mc_vec2){0, 0};
    state->cameraZoom = 1000;
    state->cameraZoomDelta = 1;

    return MC_TRUE;
}

mc_Bool frame(mcv_Canvas canvas, float dt, State* state) {
    state->cameraZoom = state->cameraZoom * pow(state->cameraZoomDelta, dt);
    state->cameraPos = (mc_vec2){
        state->cameraPos.x + state->cameraPosDelta.x * dt / state->cameraZoom,
        state->cameraPos.y + state->cameraPosDelta.y * dt / state->cameraZoom,
    };

    mc_program_set_float(state->prog, "maxIter", 500);
    mc_program_set_vec2(state->prog, "center", state->cameraPos);
    mc_program_set_float(state->prog, "zoom", state->cameraZoom);

    mc_program_dispatch(
        state->prog,
        (mc_ivec3){canvas.size.x, canvas.size.y, 1},
        1,
        (mc_Buffer*[]){canvas.buff}
    );

    char msg[100];

    sprintf(msg, "fps:  %d", (int)(1.0 / dt));
    mcv_text_tool_draw(state->textTool, canvas, msg, (mc_uvec2){20, 20});

    sprintf(msg, "pos:  %f + %fi", state->cameraPos.x, state->cameraPos.x);
    mcv_text_tool_draw(state->textTool, canvas, msg, (mc_uvec2){20, 40});

    sprintf(msg, "zoom: x%f", state->cameraZoom);
    mcv_text_tool_draw(state->textTool, canvas, msg, (mc_uvec2){20, 60});

    return MC_TRUE;
}

mc_Bool stop(mcv_Canvas canvas, State* state) {
    mc_program_destroy(state->prog);
    mcv_clear_tool_destroy(state->clearTool);
    mcv_text_tool_destroy(state->textTool);
    return MC_TRUE;
}

void key_down(mcv_Key key, State* state) {
    switch (key) {
        case MCV_KEY_UP: state->cameraPosDelta.y -= MOVE_SPEED; break;
        case MCV_KEY_DOWN: state->cameraPosDelta.y += MOVE_SPEED; break;
        case MCV_KEY_RIGHT: state->cameraPosDelta.x += MOVE_SPEED; break;
        case MCV_KEY_LEFT: state->cameraPosDelta.x -= MOVE_SPEED; break;
        case MCV_KEY_Z: state->cameraZoomDelta *= ZOOM_SPEED; break;
        case MCV_KEY_X: state->cameraZoomDelta /= ZOOM_SPEED; break;
        default: break;
    }
}

void key_up(mcv_Key key, State* state) {
    switch (key) {
        case MCV_KEY_UP: state->cameraPosDelta.y += MOVE_SPEED; break;
        case MCV_KEY_DOWN: state->cameraPosDelta.y -= MOVE_SPEED; break;
        case MCV_KEY_RIGHT: state->cameraPosDelta.x -= MOVE_SPEED; break;
        case MCV_KEY_LEFT: state->cameraPosDelta.x += MOVE_SPEED; break;
        case MCV_KEY_Z: state->cameraZoomDelta /= ZOOM_SPEED; break;
        case MCV_KEY_X: state->cameraZoomDelta *= ZOOM_SPEED; break;
        default: break;
    }
}

int main(void) {
    mcv_Settings settings = (mcv_Settings){
        .windowTitle = "Mandelbrot Test",
        .windowSize = (mc_uvec2){1000, 800},
        .canvasSize = (mc_uvec2){800, 600},
        .callbackArg = &(State){},
        .start_cb = (mcv_start_stop_cb*)start,
        .frame_cb = (mcv_frame_cb*)frame,
        .stop_cb = (mcv_start_stop_cb*)stop,
        .key_down_cb = (mcv_key_cb*)key_down,
        .key_up_cb = (mcv_key_cb*)key_up,
    };

    mc_Result res = mcv_start(settings);
    if (!res.ok) {
        mc_result_pretty_print(res);
        return 1;
    }

    return 0;
}