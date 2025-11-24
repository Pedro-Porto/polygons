#include "../include/gl_app.h"

#include <iostream>
#include <string>
#include <vector>

// ====== SHADERS DO MODE GL_POINTS (seu debug atual) ======
static const char* kVS = R"(#version 330 core
layout (location=0) in vec2 aPos;
layout (location=1) in vec4 aColor;
uniform vec2 uViewport;
out vec4 vColor;
void main(){
  float x = (aPos.x / uViewport.x) * 2.0 - 1.0;
  float y = 1.0 - (aPos.y / uViewport.y) * 2.0;
  gl_Position = vec4(x, y, 0.0, 1.0);
  vColor = aColor;
})";

static const char* kFS = R"(#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main(){ FragColor = vColor; }
)";

// ====== SHADERS DO MODE FRAMEBUFFER (NOVO) ======
static const char* kQuadVS = R"(#version 330 core
layout (location=0) in vec2 aPos;
layout (location=1) in vec2 aUV;
out vec2 vUV;
void main(){
  gl_Position = vec4(aPos, 0.0, 1.0);
  vUV = aUV;
})";

static const char* kQuadFS = R"(#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D uTex;
void main(){
  FragColor = texture(uTex, vUV);
})";

bool GLApp::init(const Config& cfg) {
    if (!glfwInit()) {
        std::cerr << "GLFW init fail\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, cfg.glMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, cfg.glMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    W = cfg.width;
    H = cfg.height;
    win = glfwCreateWindow(W, H, cfg.title, nullptr, nullptr);
    if (!win) {
        std::cerr << "Window fail\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(cfg.vsync ? 1 : 0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed\n";
        return false;
    }

    glfwSetWindowUserPointer(win, this);
    glfwSetKeyCallback(win, sKeyCB);
    glfwSetMouseButtonCallback(win, sMouseCB);
    glfwSetCursorPosCallback(win, sCursorCB);

    glViewport(0, 0, W, H);

    // ====== PROGRAMA DE PONTOS (debug) ======
    {
        GLuint vs = compile(GL_VERTEX_SHADER, kVS);
        GLuint fs = compile(GL_FRAGMENT_SHADER, kFS);
        prog = link(vs, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);
        uViewportLoc = glGetUniformLocation(prog, "uViewport");

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        const GLsizei stride = 6 * sizeof(float);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride,
                              (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    // ====== PROGRAMA DO FRAMEBUFFER (NOVO) ======
    {
        GLuint qvs = compile(GL_VERTEX_SHADER, kQuadVS);
        GLuint qfs = compile(GL_FRAGMENT_SHADER, kQuadFS);
        quadProg = link(qvs, qfs);
        glDeleteShader(qvs);
        glDeleteShader(qfs);
        uTexLoc = glGetUniformLocation(quadProg, "uTex");

        // textura do framebuffer
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        // Inicializa textura com preto ao invés de dados indefinidos
        std::vector<uint32_t> blackPixels(W * H, 0xFF000000); // RGBA preto opaco
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, W, H, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, blackPixels.data());

        // quad fullscreen (2 triângulos)
        float quad[] = {// pos      // uv
                        -1, -1, 0, 0, 1, -1, 1, 0, 1,  1, 1, 1,
                        -1, -1, 0, 0, 1, 1,  1, 1, -1, 1, 0, 1};

        glGenVertexArrays(1, &quadVao);
        glGenBuffers(1, &quadVbo);
        glBindVertexArray(quadVao);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    setClearColor(0, 0, 0, 1);
    return true;
}

void GLApp::shutdown() {
    if (prog) glDeleteProgram(prog);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);

    if (quadProg) glDeleteProgram(quadProg);
    if (quadVbo) glDeleteBuffers(1, &quadVbo);
    if (quadVao) glDeleteVertexArrays(1, &quadVao);
    if (tex) glDeleteTextures(1, &tex);

    if (win) {
        glfwDestroyWindow(win);
        win = nullptr;
    }
    glfwTerminate();
}

void GLApp::beginFrame() {
    glfwPollEvents();
    glClearColor(clearR, clearG, clearB, clearA);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLApp::drawPoints(const std::vector<float>& verts) {
    glUseProgram(prog);
    glUniform2f(uViewportLoc, (float)W, (float)H);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(verts.size() * sizeof(float)),
                 verts.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, (GLsizei)(verts.size() / 6));
    glBindVertexArray(0);
}

// NOVO: desenha framebuffer CPU como textura
void GLApp::drawFramebuffer(uint32_t* rgba, int w, int h) {
    // resize simples se o fb mudou
    if (w != W || h != H) {
        W = w;
        H = h;
        glViewport(0, 0, W, H);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, W, H, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);
    }

    // upload CPU -> GPU
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGBA, GL_UNSIGNED_BYTE,
                    rgba);

    // desenha o quad
    glUseProgram(quadProg);
    glUniform1i(uTexLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    glBindVertexArray(quadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void GLApp::endFrame() { glfwSwapBuffers(win); }

void GLApp::setClearColor(float r, float g, float b, float a) {
    clearR = r;
    clearG = g;
    clearB = b;
    clearA = a;
}

void GLApp::setKeyCallback(std::function<void(int, int, int, int)> cb) {
    onKey = std::move(cb);
}
void GLApp::setMouseButtonCallback(std::function<void(int, int, int)> cb) {
    onMouse = std::move(cb);
}
void GLApp::setCursorPosCallback(std::function<void(double, double)> cb) {
    onCursor = std::move(cb);
}

// trampolines
void GLApp::sKeyCB(GLFWwindow* w, int k, int s, int a, int m) {
    if (auto* self = static_cast<GLApp*>(glfwGetWindowUserPointer(w));
        self && self->onKey)
        self->onKey(k, s, a, m);
}
void GLApp::sMouseCB(GLFWwindow* w, int b, int a, int m) {
    if (auto* self = static_cast<GLApp*>(glfwGetWindowUserPointer(w));
        self && self->onMouse)
        self->onMouse(b, a, m);
}
void GLApp::sCursorCB(GLFWwindow* w, double x, double y) {
    if (auto* self = static_cast<GLApp*>(glfwGetWindowUserPointer(w));
        self && self->onCursor)
        self->onCursor(x, y);
}

// helpers shader
GLuint GLApp::compile(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(s, len, nullptr, log.data());
        std::cerr << "Shader compile error:\n" << log << std::endl;
    }
    return s;
}

GLuint GLApp::link(GLuint vs, GLuint fs) {
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(p, len, nullptr, log.data());
        std::cerr << "Program link error:\n" << log << std::endl;
    }
    return p;
}
