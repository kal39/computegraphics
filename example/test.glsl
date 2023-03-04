#version 460

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer buff1 {
    vec3 floatData[];
};

uniform float maxIter;
uniform vec2 center;
uniform vec2 zoom;

void main(void) {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = ivec2(gl_NumWorkGroups.xy);

    floatData[(pos.y * size.x + pos.x)] = vec3(vec2(pos) / vec2(size), 0.0);
};