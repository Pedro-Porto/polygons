#include "../include/gl_app.h"

#include <iostream>
#include <vector>
#include <cstring>


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
    glfwSetScrollCallback(win, sScrollCB);

    glViewport(0, 0, W, H);

    {
        GLuint qvs = compile(GL_VERTEX_SHADER, kQuadVS);
        GLuint qfs = compile(GL_FRAGMENT_SHADER, kQuadFS);
        quadProg = link(qvs, qfs);
        glDeleteShader(qvs);
        glDeleteShader(qfs);
        uTexLoc = glGetUniformLocation(quadProg, "uTex");

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        std::vector<uint32_t> blackPixels(static_cast<size_t>(W) * H,
                                          0xFF000000);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, W, H, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, blackPixels.data());

        float quad[] = {
            // pos      // uv
            -1, -1, 0, 0,
             1, -1, 1, 0,
             1,  1, 1, 1,

            -1, -1, 0, 0,
             1,  1, 1, 1,
            -1,  1, 0, 1
        };

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

        GLsizeiptr size = static_cast<GLsizeiptr>(W) * H * 4;
        glGenBuffers(2, pbo);
        for (int i = 0; i < 2; ++i) {
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
            glBufferData(GL_PIXEL_UNPACK_BUFFER, size, nullptr, GL_STREAM_DRAW);
        }
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        currentPBO = 0;
        firstUpload = true;
    }

    setClearColor(0, 0, 0, 1);
    return true;
}

void GLApp::shutdown() {
    if (quadProg) glDeleteProgram(quadProg);
    if (quadVbo)  glDeleteBuffers(1, &quadVbo);
    if (quadVao)  glDeleteVertexArrays(1, &quadVao);
    if (tex)      glDeleteTextures(1, &tex);

    if (pbo[0] || pbo[1]) {
        glDeleteBuffers(2, pbo);
        pbo[0] = pbo[1] = 0;
    }

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

void GLApp::drawFramebuffer(uint32_t* rgba, int w, int h) {
    if (!rgba) return;

    if (w != W || h != H) {
        W = w;
        H = h;
        glViewport(0, 0, W, H);

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, W, H, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        GLsizeiptr size = static_cast<GLsizeiptr>(W) * H * 4;
        for (int i = 0; i < 2; ++i) {
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
            glBufferData(GL_PIXEL_UNPACK_BUFFER, size, nullptr, GL_STREAM_DRAW);
        }
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        firstUpload = true;
        currentPBO  = 0;
    }

    const GLsizeiptr size = static_cast<GLsizeiptr>(W) * H * 4;

    if (firstUpload) {
        glBindTexture(GL_TEXTURE_2D, tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGBA,
                        GL_UNSIGNED_BYTE, rgba);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[0]);
        void* ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER,
                                     0, size,
                                     GL_MAP_WRITE_BIT |
                                     GL_MAP_INVALIDATE_BUFFER_BIT);
        if (ptr) {
            std::memcpy(ptr, rgba, static_cast<size_t>(size));
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        }
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        firstUpload = false;
        currentPBO  = 0;
    } else {
        int nextPBO = (currentPBO + 1) % 2;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[currentPBO]);
        glBindTexture(GL_TEXTURE_2D, tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W, H,
                        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[nextPBO]);
        void* ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER,
                                     0, size,
                                     GL_MAP_WRITE_BIT |
                                     GL_MAP_INVALIDATE_BUFFER_BIT);
        if (ptr) {
            std::memcpy(ptr, rgba, static_cast<size_t>(size));
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        }

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        currentPBO = nextPBO;
    }

    glUseProgram(quadProg);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(uTexLoc, 0);

    glBindVertexArray(quadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void GLApp::endFrame() {
    glfwSwapBuffers(win);
}

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

void GLApp::setScrollCallback(std::function<void(double, double)> cb) {
    onScroll = std::move(cb);
}

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

void GLApp::sScrollCB(GLFWwindow* w, double xoff, double yoff) {
    if (auto* self = static_cast<GLApp*>(glfwGetWindowUserPointer(w));
        self && self->onScroll)
        self->onScroll(xoff, yoff);
}


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
