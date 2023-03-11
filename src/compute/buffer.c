#include "compute_internal.h"

cg_Buffer* cg_buffer_create(uint64_t size) {
    cg_Buffer* buffer = malloc(sizeof(*buffer));
    glGenBuffers(1, &buffer->ssbo);
    cg_buffer_resize(buffer, size);
    return buffer;
}

k_Result cg_buffer_destroy(cg_Buffer* buffer) {
    K_ASSERT_ERR(buffer != NULL, "buffer is NULL");
    if (buffer->ssbo != 0) glDeleteBuffers(1, &buffer->ssbo);
    return GL_CHECK_ERROR();
}

k_Result cg_buffer_resize(cg_Buffer* buffer, uint64_t size) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    return GL_CHECK_ERROR();
}

k_Result cg_buffer_get_size(cg_Buffer* buffer, uint64_t* size) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);
    GLint iSize;
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &iSize);
    *size = iSize;
    return GL_CHECK_ERROR();
}

k_Result cg_buffer_write(
    cg_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);

    uint64_t buffSize;
    cg_buffer_get_size(buffer, &buffSize);
    K_ASSERT_ERR(
        offset + size <= buffSize,
        "offset + size larger than buffer size"
    );

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    return GL_CHECK_ERROR();
}

k_Result cg_buffer_read(
    cg_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);

    uint64_t buffSize;
    cg_buffer_get_size(buffer, &buffSize);
    K_ASSERT_ERR(
        offset + size <= buffSize,
        "offset + size larger than buffer size"
    );

    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    return GL_CHECK_ERROR();
}