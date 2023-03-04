#include "_microfb.h"

struct mf_State {
    mc_uvec2 windowSize;
    GLFWwindow* window;
    mc_uvec2 pixelBufferSize;
    mc_Buffer* pixelBuffer;
    mc_Program* pixelBufferClearProgram;
    GLuint renderProgram;
    GLuint vao;
    double prevTime;
    double dt;
};

static struct mf_State state;

static char* vertShaderCode = //
    "#version 430\n"
    "in vec3 pos;\n"
    "in vec2 vTexCoord;\n"
    "out vec2 texCoord;\n"
    "void main() {\n"
    "   gl_Position = vec4(pos.x, pos.y * 2, pos.z, 1);\n"
    "   texCoord = vTexCoord;\n"
    "}\n";

static char* fragShaderCode = //
    "#version 430\n"
    "in vec2 texCoord;\n"
    "out vec4 fragColor;\n"
    // "uniform uvec2 screenSize;\n"
    "uniform uvec2 buffSize;\n"
    "layout(std430, binding = 0) buffer ssbo0 {\n"
    "   vec3 buff[];\n"
    "};\n"
    "void main() {\n"
    // "   uvec2 pos = uvec2(buffSize * gl_FragCoord.xy);\n"
    // "   fragColor = vec4(buff[pos.y * buffSize.x + pos.x], 1.0);\n"
    "   fragColor = vec4(gl_FragCoord.xy, 0.0, 1.0);"
    "}\n";

static char* buffClearShaderCode = //
    "#version 430\n"
    "layout(local_size_x = 1, local_size_y = 1) in;\n"
    "uniform vec3 clearColor;\n"
    "layout(std430, binding = 0) buffer ssbo0 {\n"
    "    vec3 buff[];\n"
    "};\n"
    "void main() {\n"
    "   uvec2 pos = gl_GlobalInvocationID.xy;\n"
    "   uvec2 size = gl_NumWorkGroups.xy;\n"
    "   buff[pos.y * size.x + pos.x] = clearColor;\n"
    "}\n";

static mc_Result check_shader(GLuint shader, uint32_t maxLen, char* err) {
    int32_t success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) return GL_CHECK_ERROR();

    int32_t length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    ASSERT(
        (uint32_t)length < maxLen,
        "error message longer than max error buffer size"
    );
    glGetShaderInfoLog(shader, length, NULL, err);
    return ERROR("shader compile error");
}

static mc_Result check_program(GLuint program, uint32_t maxLen, char* err) {
    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success) return GL_CHECK_ERROR();

    int32_t length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    ASSERT(
        (uint32_t)length < maxLen,
        "error message longer than max error buffer size"
    );
    glGetProgramInfoLog(program, length, NULL, err);
    return ERROR("program link error");
}

mc_Result mf_initialize(
    mc_uvec2 windowSize,
    mc_uvec2 bufferSize,
    const char* windowTitle
) {
    mc_Result res;

    state.windowSize = windowSize;
    state.pixelBufferSize = bufferSize;

    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    state.window
        = glfwCreateWindow(windowSize.x, windowSize.y, windowTitle, NULL, NULL);
    glfwMakeContextCurrent(state.window);
    glewInit();
    glfwSwapInterval(0);
    // glfwSetFramebufferSizeCallback(window,
    // Pipeline::winstate.pixelBufferdow_resize_callback);

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, (const char* const*)&vertShaderCode, NULL);
    glCompileShader(vertShader);
    res = check_shader(vertShader, 0, NULL);
    ASSERT(res.ok, "failed to compile vertex shader");

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, (const char* const*)&fragShaderCode, NULL);
    glCompileShader(fragShader);
    res = check_shader(fragShader, 0, NULL);
    ASSERT(res.ok, "failed to compile fragment shader");

    state.renderProgram = glCreateProgram();
    glAttachShader(state.renderProgram, vertShader);
    glAttachShader(state.renderProgram, fragShader);
    glLinkProgram(state.renderProgram);
    res = check_program(state.renderProgram, 0, NULL);
    ASSERT(res.ok, "failed to link vertex and fragment shaders");

    GLfloat vertices[] = {
        +1.0, +1.0, 0.0, 1.0, 1.0, //
        +1.0, -1.0, 0.0, 1.0, 0.0, //
        -1.0, -1.0, 0.0, 0.0, 0.0, //
        -1.0, +1.0, 0.0, 0.0, 1.0, //
    };

    GLuint indices[] = {0, 1, 3, 1, 2, 3};

    GLuint vbo, ebo;
    glGenVertexArrays(1, &state.vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(state.vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(0)
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    state.pixelBuffer
        = mc_buffer_create(bufferSize.x * bufferSize.y * sizeof(mc_vec3));
    ASSERT(state.pixelBuffer != NULL, "failed to create pixel buffer");

    state.pixelBufferClearProgram
        = mc_program_create_from_string(buffClearShaderCode, 0, NULL);
    ASSERT(
        state.pixelBufferClearProgram != NULL,
        "failed to create pixel buffer clear program"
    );

    state.prevTime = glfwGetTime();
    return GL_CHECK_ERROR();
}

mc_Result mf_terminate() {
    glfwTerminate();
    return GL_CHECK_ERROR();
}

mc_Bool mf_window_should_close() {
    return glfwWindowShouldClose(state.window) ? MC_TRUE : MC_FALSE;
}

mc_Result mf_step() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(state.renderProgram);

    // glUniform2ui(
    //     glGetUniformLocation(state.renderProgram, "windowSize"),
    //     state.windowSize.x,
    //     state.windowSize.y
    // );

    glUniform2ui(
        glGetUniformLocation(state.renderProgram, "buffSize"),
        state.pixelBufferSize.x,
        state.pixelBufferSize.y
    );

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, *(GLuint*)(state.pixelBuffer));
    glBindVertexArray(state.vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(state.window);
    glfwPollEvents();

    double currTime = glfwGetTime();
    state.dt = currTime - state.prevTime;
    state.prevTime = currTime;

    return GL_CHECK_ERROR();
}

mc_Result mf_clear_pixel_buffer(mc_vec3 color) {
    mc_program_set_vec3(state.pixelBufferClearProgram, "clearColor", color);
    mc_program_dispatch(
        state.pixelBufferClearProgram,
        (mc_ivec3){state.pixelBufferSize.x, state.pixelBufferSize.y, 1},
        1,
        (mc_Buffer*[]){state.pixelBuffer}
    );

    return GL_CHECK_ERROR();
}

mc_Buffer* mf_get_pixel_buffer_ref() {
    return state.pixelBuffer;
}

mc_uvec2 mf_get_pixel_buffer_size() {
    return state.pixelBufferSize;
}

float mf_get_dt() {
    return state.dt;
}