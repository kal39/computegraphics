#include "_microcanvas.h"

struct mcv_State {
    mc_uvec2 windowSize;
    GLFWwindow* window;
    mcv_Canvas canvas;
    GLuint renderProg;
    GLuint vao;
};

static struct mcv_State state;

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
    "   vec4 cv[];\n"
    "};\n"
    "uniform uvec2 winSize;\n"
    "uniform uvec2 cvSize;\n"
    "void main() {\n"
    "   vec2 pos = vec2(gl_FragCoord.xy) / vec2(winSize) * vec2(cvSize);\n"
    "   pos.y = cvSize.y - pos.y;"
    "   outColor = cv[uint(pos.y) * cvSize.x + uint(pos.x)];"
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

mc_Result mcv_start(mcv_Settings settings) {
    ASSERT(
        settings.windowTitle != NULL,
        "settings option `windowTitle` is required"
    );
    ASSERT(
        memcmp(&settings.windowSize, &(mc_uvec2){0, 0}, sizeof(mc_uvec2)),
        "settings option `windowSize` is required"
    );
    ASSERT(
        memcmp(&settings.canvasSize, &(mc_uvec2){0, 0}, sizeof(mc_uvec2)),
        "settings option `canvasSize` is required"
    );
    ASSERT(
        settings.frame_cb_fn != NULL,
        "settings option `frame_cb_fn` is required"
    );

    mc_Result res;

    state.windowSize = settings.windowSize;
    state.canvas.size = settings.canvasSize;

    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    state.window = glfwCreateWindow(
        state.windowSize.x,
        state.windowSize.y,
        settings.windowTitle,
        NULL,
        NULL
    );
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

    state.canvas.buff = mc_buffer_create(
        state.canvas.size.x * state.canvas.size.y * sizeof(mc_vec4)
    );
    ASSERT(state.canvas.buff != NULL, "failed to create canvas");

    res = GL_CHECK_ERROR();
    if (!res.ok) {
        glfwTerminate();
        return res;
    }

    double prevTime = glfwGetTime();

    if (settings.start_cb_fn != NULL
        && !settings.start_cb_fn(state.canvas, settings.callbackArg)) {
        glfwTerminate();
        return ERROR("`start_cb_fn` returned MC_FALSE");
    }

    while (!glfwWindowShouldClose(state.window)) {
        double currTime = glfwGetTime();
        double dt = currTime - prevTime;
        prevTime = currTime;

        if (!settings.frame_cb_fn(state.canvas, dt, settings.callbackArg)) {
            break;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(state.renderProg);

        glUniform2ui(
            glGetUniformLocation(state.renderProg, "winSize"),
            state.windowSize.x,
            state.windowSize.y
        );

        glUniform2ui(
            glGetUniformLocation(state.renderProg, "cvSize"),
            state.canvas.size.x,
            state.canvas.size.y
        );

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, *(GLuint*)(state.canvas.buff));
        glBindVertexArray(state.vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(state.window);
        glfwPollEvents();
    }

    if (settings.stop_cb_fn != NULL
        && !settings.stop_cb_fn(state.canvas, settings.callbackArg)) {
        glfwTerminate();
        return ERROR("`stop_cb_fn` returned MC_FALSE");
    }

    glfwTerminate();
    return OK;
}