#include <math.h>
#include <microcanvas.h>
#include <stdio.h>

#define MOVE_SPEED 0.5
#define ZOOM_SPEED 2

static char* progSrc = //
    "#version 430\n"
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
    mcv_textTool* textTool;
    mc_vec2 cameraPos;
    mc_vec2 cameraPosDelta;
    float cameraZoom;
    float cameraZoomDelta;
} State;

mc_Bool start(mcv_Canvas cv, State* s) {
    int maxErrLen = 2048;
    char error[maxErrLen];

    s->prog = mc_program_from_string(progSrc, maxErrLen, error);
    if (s->prog == NULL) {
        printf("error: %s\n", error);
        return MC_FALSE;
    }

    s->textTool = mcv_text_tool_create();
    s->cameraPos = (mc_vec2){-0, -0.8};
    s->cameraPosDelta = (mc_vec2){0, 0};
    s->cameraZoom = 3;
    s->cameraZoomDelta = 1;

    return MC_TRUE;
}

mc_Bool frame(mcv_Canvas cv, float dt, State* s) {
    s->cameraZoom = s->cameraZoom * pow(s->cameraZoomDelta, dt);
    s->cameraPos = (mc_vec2){
        s->cameraPos.x + s->cameraPosDelta.x * dt / s->cameraZoom,
        s->cameraPos.y + s->cameraPosDelta.y * dt / s->cameraZoom,
    };

    mc_program_set_float(s->prog, "maxIter", 500);
    mc_program_set_vec2(s->prog, "center", s->cameraPos);
    mc_program_set_float(s->prog, "zoom", s->cameraZoom);
    mc_program_dispatch(
        s->prog,
        (mc_ivec3){cv.size.x, cv.size.y, 1},
        1,
        (mc_Buffer*[]){cv.buff}
    );

    mvc_text_tool_printf(
        s->textTool,
        cv,
        (mc_uvec2){20, 20},
        "fps:  %d\npos:  %f + %fi\nzoom: x%f",
        (int)(1.0 / dt),
        s->cameraPos.x,
        s->cameraPos.y,
        s->cameraZoom
    );

    return MC_TRUE;
}

mc_Bool stop(mcv_Canvas cv, State* s) {
    mc_program_destroy(s->prog);
    mcv_text_tool_destroy(s->textTool);
    return MC_TRUE;
}

void key_down(mcv_Key key, State* s) {
    switch (key) {
        case MCV_KEY_UP: s->cameraPosDelta.y -= MOVE_SPEED; break;
        case MCV_KEY_DOWN: s->cameraPosDelta.y += MOVE_SPEED; break;
        case MCV_KEY_RIGHT: s->cameraPosDelta.x += MOVE_SPEED; break;
        case MCV_KEY_LEFT: s->cameraPosDelta.x -= MOVE_SPEED; break;
        case MCV_KEY_Z: s->cameraZoomDelta *= ZOOM_SPEED; break;
        case MCV_KEY_X: s->cameraZoomDelta /= ZOOM_SPEED; break;
        default: break;
    }
}

void key_up(mcv_Key key, State* s) {
    switch (key) {
        case MCV_KEY_UP: s->cameraPosDelta.y += MOVE_SPEED; break;
        case MCV_KEY_DOWN: s->cameraPosDelta.y -= MOVE_SPEED; break;
        case MCV_KEY_RIGHT: s->cameraPosDelta.x -= MOVE_SPEED; break;
        case MCV_KEY_LEFT: s->cameraPosDelta.x += MOVE_SPEED; break;
        case MCV_KEY_Z: s->cameraZoomDelta /= ZOOM_SPEED; break;
        case MCV_KEY_X: s->cameraZoomDelta *= ZOOM_SPEED; break;
        default: break;
    }
}

int main(void) {
    mc_Result res = mcv_start((mcv_Settings){
        .windowTitle = "Mandelbrot Test",
        .windowSize = (mc_uvec2){1000, 800},
        .canvasSize = (mc_uvec2){1000, 800},
        .callbackArg = &(State){},
        .start_cb = (mcv_start_stop_cb*)start,
        .frame_cb = (mcv_frame_cb*)frame,
        .stop_cb = (mcv_start_stop_cb*)stop,
        .key_down_cb = (mcv_key_cb*)key_down,
        .key_up_cb = (mcv_key_cb*)key_up,
    });
    if (!res.ok) mc_result_pretty_print(res);
}