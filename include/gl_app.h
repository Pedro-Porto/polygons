#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <functional>

class GLApp {
   public:
    struct Config {
        int width = 900, height = 600;
        const char* title = "Polygons";
        int glMajor = 3, glMinor = 3;
        bool vsync = true;
    };

    bool init(const Config& cfg);
    void shutdown();

    void beginFrame();
    void drawFramebuffer(uint32_t* rgba, int w, int h);
    void endFrame();

    GLFWwindow* window() const { return win; }
    int width() const { return W; }
    int height() const { return H; }
    void setClearColor(float r, float g, float b, float a);

    void setKeyCallback(
        std::function<void(int key, int sc, int action, int mods)> cb);
    void setMouseButtonCallback(
        std::function<void(int button, int action, int mods)> cb);
    void setCursorPosCallback(std::function<void(double x, double y)> cb);
    void setScrollCallback(std::function<void(double xoffset, double yoffset)> cb);

   private:
    static void sKeyCB(GLFWwindow*, int, int, int, int);
    static void sMouseCB(GLFWwindow*, int, int, int);
    static void sCursorCB(GLFWwindow*, double, double);


    GLFWwindow* win = nullptr;
    int W = 0, H = 0;

    GLuint quadVao = 0;
    GLuint quadVbo = 0;
    GLuint quadProg = 0;
    GLuint tex = 0;
    GLint uTexLoc = -1;

    GLuint pbo[2] = {0, 0};
    int currentPBO = 0;
    bool firstUpload = true;

    float clearR = 0, clearG = 0, clearB = 0, clearA = 1;

    std::function<void(int, int, int, int)> onKey;
    std::function<void(int, int, int)> onMouse;
    std::function<void(double, double)> onCursor;
    std::function<void(double, double)> onScroll;
    
    static void sScrollCB(GLFWwindow*, double, double);

    GLuint compile(GLenum type, const char* src);
    GLuint link(GLuint vs, GLuint fs);
};
