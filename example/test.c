#include <microfb.h>
#include <stdio.h>

// static char* renderProgSrc = //
//     "#version 460\n"
//     "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
//     "layout(std430, binding = 0) buffer buff1 {\n"
//     "    vec3 floatData[];\n"
//     "};\n"
//     "uniform float maxIter;\n"
//     "uniform vec2 center;\n"
//     "uniform vec2 zoom;\n"
//     "void main(void) {\n"
//     "    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);\n"
//     "    ivec2 size = ivec2(gl_NumWorkGroups.xy);\n"
//     "    vec2 screenPos = vec2(pos) / vec2(size) - 0.5;\n"
//     "    vec2 z0 = center + screenPos / zoom;\n"
//     "    vec2 z = vec2(0.0, 0.0);\n"
//     "    vec2 z2 = vec2(0.0, 0.0);\n"
//     "    int i = 0;\n"
//     "    for(; i < maxIter && z2.x + z2.y <= 4; i++) {\n"
//     "        z = vec2(z2.x - z2.y + z0.x, 2 * z.x * z.y + z0.y);\n"
//     "        z2 = vec2(z.x * z.x, z.y * z.y);\n"
//     "    }\n"
//     "    float color = float(i) / float(maxIter);\n"
//     "    floatData[(pos.y * size.x + pos.x)] = vec3(color, \n"
//     "0.5 * color, 0.5 * color);\n"
//     "}\n";

int main(void) {
    mc_Result res;
    int maxErrLen = 2048;
    char error[maxErrLen];

    res = mf_initialize(
        (mc_uvec2){1000, 800},
        (mc_uvec2){800, 600},
        "microfb Test"
    );

    if (!res.ok) {
        mc_result_pretty_print(res);
        return -1;
    }

    mc_Program* renderProg
        = mc_program_create_from_file("example/test.glsl", maxErrLen, error);

    // mc_Program* renderProg
    //     = mc_program_create_from_string(renderProgSrc, maxErrLen, error);
    if (renderProg == NULL) {
        printf("error: %s\n", error);
        return -1;
    }

    mc_uvec2 bufferSize = mf_get_pixel_buffer_size();
    mc_Buffer* bufferRef = mf_get_pixel_buffer_ref();

    while (!mf_window_should_close()) {
        double dt = mf_get_dt();
        printf("fps: %f\n", 1.0 / dt);

        mc_program_set_float(renderProg, "maxIter", 500);
        mc_program_set_vec2(renderProg, "center", (mc_vec2){-0.7615, -0.08459});
        mc_program_set_vec2(renderProg, "zoom", (mc_vec2){1000, 1000});

        mc_program_dispatch(
            renderProg,
            (mc_ivec3){bufferSize.x, bufferSize.y, 1},
            1,
            (mc_Buffer*[]){bufferRef}
        );

        // mf_clear_pixel_buffer((mc_vec3){0, 0.5, 0.5});

        mf_step();
    }

    mf_terminate();
}