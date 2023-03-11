#include "compute_internal.h"

static void gl_debug_cb(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* msg,
    const void* data
) {
    char* sourceStr = (char*[]){
        "API",
        "WINDOW",
        "SHADER",
        "THIRD PARTY",
        "APPLICATION",
        "UNKNOWN",
    }[source - GL_DEBUG_SOURCE_API];

    char* typeStr = (char*[]){
        "error",
        "deprecated",
        "undefined",
        "portability",
        "performance",
        "other",
        "marker",
    }[type - GL_DEBUG_TYPE_ERROR];

    printf("GL: %s (%s): %s\n", sourceStr, typeStr, msg);
}

k_Result cg_memory_barrier() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    return K_OK;
}