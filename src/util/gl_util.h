#pragma once

#include <stdint.h>

#include "k_result.h"

k_Result gl_check_error(uint32_t line, const char* file, const char* func);

#define GL_CHECK_ERROR() gl_check_error(__LINE__, __FILE__, __FUNCTION__)
