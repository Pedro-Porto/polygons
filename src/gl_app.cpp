#include "../include/gl_app.h"
#include <iostream>
#include <string>

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

bool GLApp::init(const Config& cfg){
  if (!glfwInit()) { std::cerr << "GLFW init fail\n"; return false; }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, cfg.glMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, cfg.glMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  W = cfg.width; H = cfg.height;
  win = glfwCreateWindow(W, H, cfg.title, nullptr, nullptr);
  if (!win) { std::cerr << "Window fail\n"; glfwTerminate(); return false; }
  glfwMakeContextCurrent(win);
  glfwSwapInterval(cfg.vsync ? 1 : 0);

  if (!gladLoadGL()) { std::cerr << "GLAD fail\n"; return false; }

  // user pointer para callbacks
  glfwSetWindowUserPointer(win, this);
  glfwSetKeyCallback(win, sKeyCB);
  glfwSetMouseButtonCallback(win, sMouseCB);
  glfwSetCursorPosCallback(win, sCursorCB);

  glViewport(0,0,W,H);

  // shaders
  GLuint vs = compile(GL_VERTEX_SHADER,   kVS);
  GLuint fs = compile(GL_FRAGMENT_SHADER, kFS);
  prog = link(vs, fs);
  glDeleteShader(vs); glDeleteShader(fs);
  uViewportLoc = glGetUniformLocation(prog, "uViewport");

  // VAO/VBO
  glGenVertexArrays(1,&vao);
  glGenBuffers(1,&vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  const GLsizei stride = 6 * sizeof(float); // x y r g b a
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,stride,(void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,stride,(void*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  setClearColor(0,0,0,1);
  return true;
}

void GLApp::shutdown(){
  if (prog) glDeleteProgram(prog);
  if (vbo)  glDeleteBuffers(1,&vbo);
  if (vao)  glDeleteVertexArrays(1,&vao);
  if (win){ glfwDestroyWindow(win); win=nullptr; }
  glfwTerminate();
}

void GLApp::beginFrame(){
  glfwPollEvents();
  glClearColor(clearR, clearG, clearB, clearA);
  glClear(GL_COLOR_BUFFER_BIT);
}

void GLApp::drawPoints(const std::vector<float>& verts){
  glUseProgram(prog);
  glUniform2f(uViewportLoc, (float)W, (float)H);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(verts.size()*sizeof(float)),
               verts.data(), GL_DYNAMIC_DRAW);
  glDrawArrays(GL_POINTS, 0, (GLsizei)(verts.size()/6));
  glBindVertexArray(0);
}

void GLApp::endFrame(){
  glfwSwapBuffers(win);
}

void GLApp::setClearColor(float r, float g, float b, float a){
  clearR=r; clearG=g; clearB=b; clearA=a;
}

void GLApp::setKeyCallback(std::function<void(int,int,int,int)> cb){ onKey = std::move(cb); }
void GLApp::setMouseButtonCallback(std::function<void(int,int,int)> cb){ onMouse = std::move(cb); }
void GLApp::setCursorPosCallback(std::function<void(double,double)> cb){ onCursor = std::move(cb); }

// trampolines
void GLApp::sKeyCB(GLFWwindow* w, int k,int s,int a,int m){
  if (auto* self = static_cast<GLApp*>(glfwGetWindowUserPointer(w)); self && self->onKey) self->onKey(k,s,a,m);
}
void GLApp::sMouseCB(GLFWwindow* w, int b,int a,int m){
  if (auto* self = static_cast<GLApp*>(glfwGetWindowUserPointer(w)); self && self->onMouse) self->onMouse(b,a,m);
}
void GLApp::sCursorCB(GLFWwindow* w, double x,double y){
  if (auto* self = static_cast<GLApp*>(glfwGetWindowUserPointer(w)); self && self->onCursor) self->onCursor(x,y);
}

// helpers shader
GLuint GLApp::compile(GLenum type, const char* src){
  GLuint s = glCreateShader(type);
  glShaderSource(s,1,&src,nullptr);
  glCompileShader(s);
  GLint ok=0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if(!ok){
    GLint len=0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
    std::string log(len,'\0'); glGetShaderInfoLog(s,len,nullptr,log.data());
    std::cerr << "Shader compile error:\n" << log << std::endl;
  }
  return s;
}
GLuint GLApp::link(GLuint vs, GLuint fs){
  GLuint p = glCreateProgram();
  glAttachShader(p,vs); glAttachShader(p,fs);
  glLinkProgram(p);
  GLint ok=0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
  if(!ok){
    GLint len=0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
    std::string log(len,'\0'); glGetProgramInfoLog(p,len,nullptr,log.data());
    std::cerr << "Program link error:\n" << log << std::endl;
  }
  return p;
}
