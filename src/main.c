#include <math.h>
#include <stdio.h>

#include "compute/compute.h"
#include "graphics/graphics.h"

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
    cg_Program* prog;
    k_Bool stop;
    cg_vec2 cameraPos;
    cg_vec2 cameraPosDelta;
    float cameraZoom;
    float cameraZoomDelta;
} State;

k_Bool start(cg_Canvas* cv, State* s) {
    int maxErrLen = 2048;
    char error[maxErrLen];

    s->prog = cg_program_from_string(progSrc, maxErrLen, error);
    if (s->prog == NULL) {
        printf("error: %s\n", error);
        return K_FALSE;
    }

    s->stop = K_FALSE;
    s->cameraPos = (cg_vec2){-0, -0.8};
    s->cameraPosDelta = (cg_vec2){0, 0};
    s->cameraZoom = 3;
    s->cameraZoomDelta = 1;

    return K_TRUE;
}

k_Bool frame(cg_Canvas* cv, float dt, State* s) {
    s->cameraZoom = s->cameraZoom * pow(s->cameraZoomDelta, dt);
    s->cameraPos = (cg_vec2){
        s->cameraPos.x + s->cameraPosDelta.x * dt / s->cameraZoom,
        s->cameraPos.y + s->cameraPosDelta.y * dt / s->cameraZoom,
    };

    cg_program_set_float(s->prog, "maxIter", 500);
    cg_program_set_vec2(s->prog, "center", s->cameraPos);
    cg_program_set_float(s->prog, "zoom", s->cameraZoom);
    cg_program_dispatch(
        s->prog,
        (cg_ivec3){cg_canvas_get_size(cv).x, cg_canvas_get_size(cv).y, 1},
        1,
        (cg_Buffer*[]){cg_canvas_get_buff(cv)}
    );

    cg_canvas_printf(
        cv,
        (cg_uvec2){20, 20},
        2,
        (cg_uvec2){2, 2},
        (cg_vec4){0.8, 0.8, 0.8, 1.0},
        4,
        "fps:  %d\npos:  %f + %fi\nzoom: x%f",
        (int)(1.0 / dt),
        s->cameraPos.x,
        s->cameraPos.y,
        s->cameraZoom
    );

    return !s->stop;
}

k_Bool stop(cg_Canvas* cv, State* s) {
    cg_program_destroy(s->prog);
    return K_TRUE;
}

void key_down(cg_Key key, State* s) {
    switch (key) {
        case MCV_KEY_UP: s->cameraPosDelta.y -= MOVE_SPEED; break;
        case MCV_KEY_DOWN: s->cameraPosDelta.y += MOVE_SPEED; break;
        case MCV_KEY_RIGHT: s->cameraPosDelta.x += MOVE_SPEED; break;
        case MCV_KEY_LEFT: s->cameraPosDelta.x -= MOVE_SPEED; break;
        case MCV_KEY_Z: s->cameraZoomDelta *= ZOOM_SPEED; break;
        case MCV_KEY_X: s->cameraZoomDelta /= ZOOM_SPEED; break;
        case MCV_KEY_ESCAPE: s->stop = K_TRUE; break;
        default: break;
    }
}

void key_up(cg_Key key, State* s) {
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
    k_Result res = cg_start((cg_Settings){
        .windowTitle = "mcv test",
        .windowSize = (cg_uvec2){1000, 800},
        .resizable = K_TRUE,
        .canvasSize = (cg_uvec2){1000, 800},
        .callbackArg = &(State){},
        .start_cb = (cg_start_stop_cb*)start,
        .frame_cb = (cg_frame_cb*)frame,
        .stop_cb = (cg_start_stop_cb*)stop,
        .key_down_cb = (cg_key_cb*)key_down,
        .key_up_cb = (cg_key_cb*)key_up,
    });
    if (!res.ok) K_RESULT_PRINT(res);
}
