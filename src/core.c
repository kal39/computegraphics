#include "_microfb.h"

struct mf_State {
    mc_uvec2 windowSize;
    GLFWwindow* window;
    mc_uvec2 buffSize;
    mc_Buffer* buff;
    mc_Program* buffClearProg;
    GLuint renderProg;
    GLuint vao;
    double prevTime;
    double dt;
};

static struct mf_State state;

static char* vertCode = //
    "#version 430\n"
    "layout (location = 0) in vec2 inPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(inPos, 0.0, 1.0);\n"
    "}\n";

static char* fragCode = //
    "#version 430\n"
    "layout (location = 0) out vec4 outColor;"
    "layout(std430, binding = 0) buffer ssbo0 {\n"
    "   vec4 buff[];\n"
    "};\n"
    "uniform uvec2 windowSize;\n"
    "uniform uvec2 buffSize;\n"
    "void main() {\n"
    "   vec2 pos = vec2(gl_FragCoord.xy) / vec2(windowSize) * vec2(buffSize);\n"
    "   outColor = buff[uint(pos.y) * buffSize.x + uint(pos.x)];"
    "}\n";

static char* clearCode = //
    "#version 430\n"
    "layout(local_size_x = 1, local_size_y = 1) in;\n"
    "uniform vec4 clearColor;\n"
    "layout(std430, binding = 0) buffer ssbo0 {\n"
    "    vec4 buff[];\n"
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
        "error message longer than max error buff size"
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
        "error message longer than max error buff size"
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
    state.buffSize = bufferSize;

    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    state.window
        = glfwCreateWindow(windowSize.x, windowSize.y, windowTitle, NULL, NULL);
    glfwMakeContextCurrent(state.window);
    glewInit();
    glfwSwapInterval(0);

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, (const char* const*)&vertCode, NULL);
    glCompileShader(vertShader);
    res = check_shader(vertShader, 0, NULL);
    ASSERT(res.ok, "failed to compile vertex shader");

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, (const char* const*)&fragCode, NULL);
    glCompileShader(fragShader);
    res = check_shader(fragShader, 0, NULL);
    ASSERT(res.ok, "failed to compile fragment shader");

    state.renderProg = glCreateProgram();
    glAttachShader(state.renderProg, vertShader);
    glAttachShader(state.renderProg, fragShader);
    glLinkProgram(state.renderProg);
    res = check_program(state.renderProg, 0, NULL);
    ASSERT(res.ok, "failed to link vertex and fragment shaders");

    GLfloat vertices[] = {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

    GLuint vbo;
    glGenVertexArrays(1, &state.vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(state.vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    state.buff = mc_buffer_create(
        state.buffSize.x * state.buffSize.y * sizeof(mc_vec4)
    );
    ASSERT(state.buff != NULL, "failed to create pixel buff");

    state.buffClearProg = mc_program_create_from_string(clearCode, 0, NULL);
    ASSERT(state.buffClearProg != NULL, "failed to create clear program");

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

    glUseProgram(state.renderProg);

    glUniform2ui(
        glGetUniformLocation(state.renderProg, "windowSize"),
        state.windowSize.x,
        state.windowSize.y
    );

    glUniform2ui(
        glGetUniformLocation(state.renderProg, "buffSize"),
        state.buffSize.x,
        state.buffSize.y
    );

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, *(GLuint*)(state.buff));
    glBindVertexArray(state.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glfwSwapBuffers(state.window);
    glfwPollEvents();

    double currTime = glfwGetTime();
    state.dt = currTime - state.prevTime;
    state.prevTime = currTime;

    return GL_CHECK_ERROR();
}

mc_Result mf_clear_buffer(mc_vec4 color) {
    mc_program_set_vec4(state.buffClearProg, "clearColor", color);
    mc_program_dispatch(
        state.buffClearProg,
        (mc_ivec3){state.buffSize.x, state.buffSize.y, 1},
        1,
        (mc_Buffer*[]){state.buff}
    );

    return GL_CHECK_ERROR();
}

mc_Buffer* mf_get_buffer_ref() {
    return state.buff;
}

mc_uvec2 mf_get_buffer_size() {
    return state.buffSize;
}

float mf_get_dt() {
    return state.dt;
}