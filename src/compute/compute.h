#pragma once

#include <stdint.h>

#include "k_result.h"

typedef struct cg_Program cg_Program;
typedef struct cg_Buffer cg_Buffer;

typedef struct cg_vec2 {
    float x, y;
} cg_vec2;

typedef struct cg_vec3 {
    float x, y, z;
} cg_vec3;

typedef struct cg_vec4 {
    float x, y, z, w;
} cg_vec4;

typedef struct cg_ivec2 {
    int32_t x, y;
} cg_ivec2;

typedef struct cg_ivec3 {
    int32_t x, y, z;
} cg_ivec3;

typedef struct cg_ivec4 {
    int32_t x, y, z, w;
} cg_ivec4;

typedef struct cg_uvec2 {
    uint32_t x, y;
} cg_uvec2;

typedef struct cg_uvec3 {
    uint32_t x, y, z;
} cg_uvec3;

typedef struct cg_uvec4 {
    uint32_t x, y, z, w;
} cg_uvec4;

typedef struct cg_mat22 {
    float values[4];
    k_Bool transpose;
} cg_mat22;

typedef struct cg_mat33 {
    float values[9];
    k_Bool transpose;
} cg_mat33;

typedef struct cg_mat44 {
    float values[16];
    k_Bool transpose;
} cg_mat44;

typedef struct cg_mat23 {
    float values[6];
    k_Bool transpose;
} cg_mat23;

typedef struct cg_mat32 {
    float values[6];
    k_Bool transpose;
} cg_mat32;

typedef struct cg_mat24 {
    float values[8];
    k_Bool transpose;
} cg_mat24;

typedef struct cg_mat42 {
    float values[8];
    k_Bool transpose;
} cg_mat42;

typedef struct cg_mat34 {
    float values[12];
    k_Bool transpose;
} cg_mat34;

typedef struct cg_mat43 {
    float values[12];
    k_Bool transpose;
} cg_mat43;

typedef enum cg_DebugLevel {
    cg_DebugLevel_INFO,
    cg_DebugLevel_LOW,
    cg_DebugLevel_MEDIUM,
    cg_DebugLevel_HIGH,
} cg_DebugLevel;

k_Result cg_start();

k_Result cg_stop();

k_Result cg_memory_barrier();

cg_Program* cg_program_from_string(
    const char* code,
    uint32_t maxErrorLength,
    char* error
);

cg_Program* cg_program_from_file(
    const char* path,
    uint32_t maxErrorLength,
    char* error
);

k_Result cg_program_destroy(cg_Program* program);

k_Result cg_program_dispatch(
    cg_Program* program,
    cg_ivec3 size,
    uint32_t bufferCount,
    cg_Buffer** buffers
);

k_Result cg_program_set_float(cg_Program* program, char* name, float value);
k_Result cg_program_set_vec2(cg_Program* program, char* name, cg_vec2 value);
k_Result cg_program_set_vec3(cg_Program* program, char* name, cg_vec3 value);
k_Result cg_program_set_vec4(cg_Program* program, char* name, cg_vec4 value);

k_Result cg_program_set_int(cg_Program* program, char* name, int32_t value);
k_Result cg_program_set_ivec2(cg_Program* program, char* name, cg_ivec2 value);
k_Result cg_program_set_ivec3(cg_Program* program, char* name, cg_ivec3 value);
k_Result cg_program_set_ivec4(cg_Program* program, char* name, cg_ivec4 value);

k_Result cg_program_set_uint(cg_Program* program, char* name, uint32_t value);
k_Result cg_program_set_uvec2(cg_Program* program, char* name, cg_uvec2 value);
k_Result cg_program_set_uvec3(cg_Program* program, char* name, cg_uvec3 value);
k_Result cg_program_set_uvec4(cg_Program* program, char* name, cg_uvec4 value);

k_Result cg_program_set_mat22(cg_Program* program, char* name, cg_mat22 value);
k_Result cg_program_set_mat33(cg_Program* program, char* name, cg_mat33 value);
k_Result cg_program_set_mat44(cg_Program* program, char* name, cg_mat44 value);
k_Result cg_program_set_mat23(cg_Program* program, char* name, cg_mat23 value);
k_Result cg_program_set_mat32(cg_Program* program, char* name, cg_mat32 value);
k_Result cg_program_set_mat24(cg_Program* program, char* name, cg_mat24 value);
k_Result cg_program_set_mat42(cg_Program* program, char* name, cg_mat42 value);
k_Result cg_program_set_mat34(cg_Program* program, char* name, cg_mat34 value);
k_Result cg_program_set_mat43(cg_Program* program, char* name, cg_mat43 value);

cg_Buffer* cg_buffer_create(uint64_t size);

k_Result cg_buffer_destroy(cg_Buffer* buffer);

k_Result cg_buffer_resize(cg_Buffer* buffer, uint64_t size);

k_Result cg_buffer_get_size(cg_Buffer* buffer, uint64_t* size);

k_Result cg_buffer_write(
    cg_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);

k_Result cg_buffer_read(
    cg_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);
