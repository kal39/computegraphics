#include "graphics_internal.h"

struct cg_State {
    cg_uvec2 windowSize;
    cg_Canvas* canvas;
    void* arg;
    cg_window_resize_cb* resize_cb;
    cg_start_stop_cb* start_cb;
    cg_frame_cb* frame_cb;
    cg_start_stop_cb* stop_cb;
    cg_key_cb* key_down_cb;
    cg_key_cb* key_up_cb;
    cg_mouse_cb* mouse_move_cb;
    cg_mouse_cb* mouse_scroll_cb;
};

static struct cg_State state;

static char* vertCode = //
    "#version 430\n"
    "layout (location = 0) in vec2 inPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(inPos, 0.0, 1.0);\n"
    "}\n";

static char* fragCode = //
    "#version 430\n"
    "layout (location = 0) out vec4 outColor;"
    "layout(std430, binding = 0) restrict coherent readonly buffer ssbo0 {\n"
    "   vec4 cv[];\n"
    "};\n"
    "uniform uvec2 winSize;\n"
    "uniform uvec2 cvSize;\n"
    "void main() {\n"
    "   vec2 pos = vec2(gl_FragCoord.xy) / vec2(winSize) * vec2(cvSize);\n"
    "   pos.y = cvSize.y - pos.y;"
    "   outColor = cv[uint(pos.y) * cvSize.x + uint(pos.x)];"
    "}\n";

static k_Result check_shader(GLuint shader, uint32_t maxLen, char* err) {
    int32_t success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) return GL_CHECK_ERROR();

    int32_t length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    K_ASSERT_ERR(
        (uint32_t)length < maxLen,
        "error message longer than max error buff size"
    );
    glGetShaderInfoLog(shader, length, NULL, err);
    return K_ERROR("shader compile error");
}

static k_Result check_program(GLuint program, uint32_t maxLen, char* err) {
    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success) return GL_CHECK_ERROR();

    int32_t length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    K_ASSERT_ERR(
        (uint32_t)length < maxLen,
        "error message longer than max error buff size"
    );
    glGetProgramInfoLog(program, length, NULL, err);
    return K_ERROR("program link error");
}

static void key_cb(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
) {
    switch (action) {
        case GLFW_PRESS:
            if (state.key_down_cb != NULL) state.key_down_cb(key, state.arg);
            break;
        case GLFW_RELEASE:
            if (state.key_up_cb != NULL) state.key_up_cb(key, state.arg);
            break;
        default: break;
    }
}

static void mouse_btn_cb(GLFWwindow* window, int btn, int action, int mods) {
    btn += 400;
    switch (action) {
        case GLFW_PRESS:
            if (state.key_down_cb != NULL) state.key_down_cb(btn, state.arg);
            break;
        case GLFW_RELEASE:
            if (state.key_up_cb != NULL) state.key_up_cb(btn, state.arg);
            break;
        default: break;
    }
}

static void mouse_mv_cb(GLFWwindow* window, double x, double y) {
    if (state.mouse_move_cb != NULL)
        state.mouse_move_cb((cg_vec2){x, y}, state.arg);
}

static void mouse_sc_cb(GLFWwindow* window, double x, double y) {
    if (state.mouse_scroll_cb != NULL)
        state.mouse_scroll_cb((cg_vec2){x, y}, state.arg);
}

static void window_rs_cb(GLFWwindow* window, int width, int height) {
    state.windowSize = (cg_uvec2){width, height};
    glViewport(0, 0, state.windowSize.x, state.windowSize.y);

    if (state.resize_cb != NULL)
        state.resize_cb((cg_uvec2){width, height}, state.arg);
}

k_Result cg_start(cg_Settings settings) {
    K_ASSERT_ERR(
        settings.windowTitle != NULL,
        "settings option windowTitle is has not been set"
    );
    K_ASSERT_ERR(
        memcmp(&settings.windowSize, &(cg_uvec2){0, 0}, sizeof(cg_uvec2)),
        "settings option windowSize is has not been set"
    );
    K_ASSERT_ERR(
        memcmp(&settings.canvasSize, &(cg_uvec2){0, 0}, sizeof(cg_uvec2)),
        "settings option canvasSize is has not been set"
    );
    K_ASSERT_ERR(
        settings.frame_cb != NULL,
        "settings option frame_cb is has not been set"
    );

    k_Result res;

    state.windowSize = settings.windowSize;
    state.arg = settings.callbackArg;
    state.start_cb = settings.start_cb;
    state.frame_cb = settings.frame_cb;
    state.stop_cb = settings.stop_cb;
    state.key_down_cb = settings.key_down_cb;
    state.key_up_cb = settings.key_up_cb;
    state.mouse_move_cb = settings.mouse_move_cb;
    state.mouse_scroll_cb = settings.mouse_scroll_cb;

    glfwInit();

    GLFWwindow* window;
    if (settings.fullScreen) {
        window = glfwCreateWindow(
            glfwGetVideoMode(glfwGetPrimaryMonitor())->width,
            glfwGetVideoMode(glfwGetPrimaryMonitor())->height,
            settings.windowTitle,
            glfwGetPrimaryMonitor(),
            NULL
        );
    } else {
        glfwWindowHint(GLFW_RESIZABLE, settings.resizable);
        window = glfwCreateWindow(
            state.windowSize.x,
            state.windowSize.y,
            settings.windowTitle,
            NULL,
            NULL
        );
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    glfwSetKeyCallback(window, key_cb);
    glfwSetMouseButtonCallback(window, mouse_btn_cb);
    glfwSetCursorPosCallback(window, mouse_mv_cb);
    glfwSetScrollCallback(window, mouse_sc_cb);
    glfwSetFramebufferSizeCallback(window, window_rs_cb);

    K_ASSERT_ERR(gladLoadGL() != 0, "failed to load GLAD");

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, (const char* const*)&vertCode, NULL);
    glCompileShader(vertShader);
    res = check_shader(vertShader, 0, NULL);
    K_ASSERT_ERR(res.ok, "failed to compile vertex shader");
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, (const char* const*)&fragCode, NULL);
    glCompileShader(fragShader);
    res = check_shader(fragShader, 0, NULL);
    K_ASSERT_ERR(res.ok, "failed to compile fragment shader");

    GLuint renderProg = glCreateProgram();
    glAttachShader(renderProg, vertShader);
    glAttachShader(renderProg, fragShader);
    glLinkProgram(renderProg);
    res = check_program(renderProg, 0, NULL);
    K_ASSERT_ERR(res.ok, "failed to link vertex and fragment shaders");

    GLfloat vertices[] = {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    state.canvas = cg_canvas_create(settings.canvasSize);
    K_ASSERT_ERR(state.canvas != NULL, "failed to create canvas");

    res = GL_CHECK_ERROR();
    K_ASSERT_DO_RET(res.ok, glfwTerminate(), res);

    double prevTime = glfwGetTime();
    K_ASSERT_ERR(
        state.start_cb == NULL || state.start_cb(state.canvas, state.arg),
        "start_cb failed"
    );

    while (!glfwWindowShouldClose(window)) {
        double currTime = glfwGetTime();
        double dt = currTime - prevTime;
        prevTime = currTime;

        if (!state.frame_cb(state.canvas, dt, state.arg)) break;

        cg_memory_barrier();

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(renderProg);

        glUniform2ui(
            glGetUniformLocation(renderProg, "winSize"),
            state.windowSize.x,
            state.windowSize.y
        );

        glUniform2ui(
            glGetUniformLocation(renderProg, "cvSize"),
            state.canvas->size.x,
            state.canvas->size.y
        );

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, *(GLuint*)(state.canvas->buff));
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    K_ASSERT_ERR(
        state.stop_cb == NULL || state.stop_cb(state.canvas, state.arg),
        "stop_cb failed"
    );

    cg_canvas_destroy(state.canvas);
    glfwTerminate();
    return res;
}