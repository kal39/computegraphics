#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "_glad/glad.h"

#include "compute.h"
#include "util/gl_util.h"

typedef struct cg_Program {
    GLint shader;
    GLint program;
} cg_Program;

typedef struct cg_Buffer {
    GLuint ssbo;
} cg_Buffer;