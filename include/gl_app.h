#pragma once
#include <functional>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

  // ciclo de frame
  void beginFrame();
  void drawPoints(const std::vector<float>& verts); // verts: [x,y,r,g,b,a]
  void endFrame();

  // utilidades
  GLFWwindow* window() const { return win; }
  int width()  const { return W; }
  int height() const { return H; }
  void setClearColor(float r, float g, float b, float a);

  // callbacks “do usuário” (sua lógica)
  void setKeyCallback(std::function<void(int key,int sc,int action,int mods)> cb);
  void setMouseButtonCallback(std::function<void(int button,int action,int mods)> cb);
  void setCursorPosCallback(std::function<void(double x,double y)> cb);

private:
  // trampolines estáticos -> repassam para this via userPointer
  static void sKeyCB(GLFWwindow*, int, int, int, int);
  static void sMouseCB(GLFWwindow*, int, int, int);
  static void sCursorCB(GLFWwindow*, double, double);

  // estado GL
  GLFWwindow* win = nullptr;
  int W=0, H=0;
  GLuint vao=0, vbo=0, prog=0;
  GLint  uViewportLoc=-1;
  float clearR=0, clearG=0, clearB=0, clearA=1;

  // callbacks do usuário
  std::function<void(int,int,int,int)>  onKey;
  std::function<void(int,int,int)>      onMouse;
  std::function<void(double,double)>    onCursor;

  // helpers shader
  GLuint compile(GLenum type, const char* src);
  GLuint link(GLuint vs, GLuint fs);
};
