#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "microcanvas.h"

typedef struct mcv_clearTool {
    mc_Program* program;
} mcv_clearTool;

typedef struct mcv_textTool {
    mc_Program* program;
    uint32_t scale;
    mc_uvec2 spacing;
} mcv_textTool;

mc_Result gl_check_error(uint32_t line, const char* file, const char* func);

#define GL_CHECK_ERROR() gl_check_error(__LINE__, __FILE__, __FUNCTION__)

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
