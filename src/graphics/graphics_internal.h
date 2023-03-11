#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_glad/glad.h"

#include "GLFW/glfw3.h"
#include "graphics.h"
#include "util/gl_util.h"

typedef struct cg_Canvas {
    cg_uvec2 size;
    cg_Buffer* buff;
    cg_Program* clearProgram;
    cg_Program* textProgram;
} cg_Canvas;
