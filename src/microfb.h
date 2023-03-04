#pragma once

#include <microcompute.h>

mc_Result mf_initialize(
    mc_uvec2 windowSize,
    mc_uvec2 bufferSize,
    const char* title
);

mc_Result mf_terminate();

mc_Bool mf_window_should_close();
mc_Result mf_step();

mc_Result mf_clear_pixel_buffer(mc_vec3 color);
mc_Buffer* mf_get_pixel_buffer_ref();
mc_uvec2 mf_get_pixel_buffer_size();

float mf_get_dt();