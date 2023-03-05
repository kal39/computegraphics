#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "microcanvas.h"

typedef struct mcv_canvasClearTool {
    mc_Program* program;
} mcv_canvasClearTool;

typedef struct mcv_textDrawTool {
    mc_Program* program;
    mc_Buffer* fontData;
    uint32_t scale;
    uint32_t spacing;
} mcv_textDrawTool;

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
