#include "compute_internal.h"

#define MC_SET_VALUE(program, name, set_fn, ...)                               \
    GLint loc = glGetUniformLocation(program->program, name);                  \
    K_ASSERT_ERR(loc != -1, "failed to find uniform");                         \
    glUseProgram(program->program);                                            \
    set_fn(loc, __VA_ARGS__);                                                  \
    return GL_CHECK_ERROR();

#define MC_SET_MATRIX(program, name, set_fn, v)                                \
    MC_SET_VALUE(program, name, set_fn, 1, v.transpose, v.values)

static char* cg_read_file(const char* path, uint32_t* size) {
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) return NULL;

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp) + 1;
    fseek(fp, 0, SEEK_SET);

    char* contents = malloc(length);
    length = fread(contents, 1, length, fp);
    contents[length - 1] = '\0';
    fclose(fp);

    if (size != NULL) *size = length;
    return contents;
}

static k_Result check_shader(GLuint shader, uint32_t maxLen, char* err) {
    int32_t success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) return GL_CHECK_ERROR();

    int32_t length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    K_ASSERT_ERR(
        (uint32_t)length < maxLen,
        "error message longer than max error buffer size"
    );
    glGetShaderInfoLog(shader, length, NULL, err);
    return K_ERROR("shader compile error");
}

static k_Result check_program(GLuint program, uint32_t maxLen, char* err) {
    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success) return GL_CHECK_ERROR();

    int32_t length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    K_ASSERT_ERR(
        (uint32_t)length < maxLen,
        "error message longer than max error buffer size"
    );
    glGetProgramInfoLog(program, length, NULL, err);
    return K_ERROR("program link error");
}

cg_Program* cg_program_from_string(
    const char* code,
    uint32_t maxErrorLength,
    char* error
) {
    cg_Program* program = malloc(sizeof(*program));

    program->shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(program->shader, 1, &code, NULL);
    glCompileShader(program->shader);

    k_Result res = check_shader(program->shader, maxErrorLength, error);
    K_ASSERT_DO_RET(res.ok, free(program), NULL);

    program->program = glCreateProgram();
    glAttachShader(program->program, program->shader);
    glLinkProgram(program->program);

    res = check_program(program->program, maxErrorLength, error);
    K_ASSERT_DO_RET(res.ok, free(program), NULL);

    return program;
}

cg_Program* cg_program_from_file(
    const char* path,
    uint32_t maxErrorLength,
    char* error
) {
    char* code = cg_read_file(path, NULL);
    K_ASSERT_RET(code != NULL, NULL);

    cg_Program* program = cg_program_from_string(code, maxErrorLength, error);
    free(code);
    return program;
}

k_Result cg_program_destroy(cg_Program* program) {
    K_ASSERT_ERR(program != NULL, "program is NULL");
    if (program->shader != 0) glDeleteShader(program->shader);
    if (program->program != 0) glDeleteProgram(program->program);
    return GL_CHECK_ERROR();
}

k_Result cg_program_dispatch(
    cg_Program* program,
    cg_ivec3 size,
    uint32_t bufferCount,
    cg_Buffer** buffers
) {
    for (uint32_t i = 0; i < bufferCount; i++) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[i]->ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, buffers[i]->ssbo);
    }

    glUseProgram(program->program);
    glDispatchCompute(size.x, size.y, size.z);
    return GL_CHECK_ERROR();
}

k_Result cg_program_set_float(cg_Program* program, char* name, float v) {
    MC_SET_VALUE(program, name, glUniform1f, v);
}

k_Result cg_program_set_vec2(cg_Program* program, char* name, cg_vec2 v) {
    MC_SET_VALUE(program, name, glUniform2f, v.x, v.y);
}

k_Result cg_program_set_vec3(cg_Program* program, char* name, cg_vec3 v) {
    MC_SET_VALUE(program, name, glUniform3f, v.x, v.y, v.z);
}

k_Result cg_program_set_vec4(cg_Program* program, char* name, cg_vec4 v) {
    MC_SET_VALUE(program, name, glUniform4f, v.x, v.y, v.z, v.w);
}

k_Result cg_program_set_int(cg_Program* program, char* name, int32_t v) {
    MC_SET_VALUE(program, name, glUniform1i, v);
}

k_Result cg_program_set_ivec2(cg_Program* program, char* name, cg_ivec2 v) {
    MC_SET_VALUE(program, name, glUniform2i, v.x, v.y);
}

k_Result cg_program_set_ivec3(cg_Program* program, char* name, cg_ivec3 v) {
    MC_SET_VALUE(program, name, glUniform3i, v.x, v.y, v.z);
}

k_Result cg_program_set_ivec4(cg_Program* program, char* name, cg_ivec4 v) {
    MC_SET_VALUE(program, name, glUniform4i, v.x, v.y, v.z, v.w);
}

k_Result cg_program_set_uint(cg_Program* program, char* name, uint32_t v) {
    MC_SET_VALUE(program, name, glUniform1ui, v);
}

k_Result cg_program_set_uvec2(cg_Program* program, char* name, cg_uvec2 v) {
    MC_SET_VALUE(program, name, glUniform2ui, v.x, v.y);
}

k_Result cg_program_set_uvec3(cg_Program* program, char* name, cg_uvec3 v) {
    MC_SET_VALUE(program, name, glUniform3ui, v.x, v.y, v.z);
}

k_Result cg_program_set_uvec4(cg_Program* program, char* name, cg_uvec4 v) {
    MC_SET_VALUE(program, name, glUniform4ui, v.x, v.y, v.z, v.w);
}

k_Result cg_program_set_mat22(cg_Program* program, char* name, cg_mat22 v) {
    MC_SET_MATRIX(program, name, glUniformMatrix2fv, v);
}

k_Result cg_program_set_mat33(cg_Program* program, char* name, cg_mat33 v) {
    MC_SET_MATRIX(program, name, glUniformMatrix3fv, v);
}

k_Result cg_program_set_mat44(cg_Program* program, char* name, cg_mat44 v) {
    MC_SET_MATRIX(program, name, glUniformMatrix4fv, v);
}

k_Result cg_program_set_mat23(cg_Program* program, char* name, cg_mat23 v) {
    MC_SET_MATRIX(program, name, glUniformMatrix2x3fv, v);
}

k_Result cg_program_set_mat32(cg_Program* program, char* name, cg_mat32 v) {
    MC_SET_MATRIX(program, name, glUniformMatrix3x2fv, v);
}

k_Result cg_program_set_mat24(cg_Program* program, char* name, cg_mat24 v) {
    MC_SET_MATRIX(program, name, glUniformMatrix2x4fv, v);
}

k_Result cg_program_set_mat42(cg_Program* program, char* name, cg_mat42 v) {
    MC_SET_MATRIX(program, name, glUniformMatrix4x2fv, v);
}

k_Result cg_program_set_mat34(cg_Program* program, char* name, cg_mat34 v) {
    MC_SET_MATRIX(program, name, glUniformMatrix3x4fv, v);
}

k_Result cg_program_set_mat43(cg_Program* program, char* name, cg_mat43 v) {
    MC_SET_MATRIX(program, name, glUniformMatrix4x3fv, v);
}