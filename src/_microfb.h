#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "microfb.h"

#define GL_CHECK_ERROR() gl_check_error(__LINE__, __FILE__, __FUNCTION__)

mc_Result gl_check_error(uint32_t line, const char* file, const char* func);

#define OK                                                                     \
    ((mc_Result){                                                              \
        .ok = MC_TRUE,                                                         \
        .file = __FILE__,                                                      \
        .line = __LINE__,                                                      \
        .func = __FUNCTION__,                                                  \
        .message = (char*){"no errors here :)"}})

#define ERROR(msg)                                                             \
    ((mc_Result){                                                              \
        .ok = MC_FALSE,                                                        \
        .file = __FILE__,                                                      \
        .line = __LINE__,                                                      \
        .func = __FUNCTION__,                                                  \
        .message = (char*){(msg)}})

#define ASSERT(cond, msg)                                                      \
    do {                                                                       \
        if (!(cond)) return ERROR((msg));                                      \
    } while (0)