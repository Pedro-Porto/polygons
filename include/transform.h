#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Funções simples pra montar Model / View / Projection usando GLM.
// Você usa no CPU e aplica no seu pipeline (Framebuffer).

inline glm::mat4 modelMatrix(glm::vec3 pos,
                             glm::vec3 rotAxis, float rotAngleRad,
                             glm::vec3 scaleVec)
{
    glm::mat4 M(1.0f);
    M = glm::translate(M, pos);
    if (rotAngleRad != 0.0f)
        M = glm::rotate(M, rotAngleRad, rotAxis);
    M = glm::scale(M, scaleVec);
    return M;
}

// alternativa com rotações separadas (rad)
inline glm::mat4 modelMatrixEuler(glm::vec3 pos,
                                  glm::vec3 rotRad,    // (rx,ry,rz)
                                  glm::vec3 scaleVec)
{
    glm::mat4 M(1.0f);
    M = glm::translate(M, pos);
    M = glm::rotate(M, rotRad.x, glm::vec3(1,0,0));
    M = glm::rotate(M, rotRad.y, glm::vec3(0,1,0));
    M = glm::rotate(M, rotRad.z, glm::vec3(0,0,1));
    M = glm::scale(M, scaleVec);
    return M;
}

// View e Projection normalmente vêm da Camera.
// Deixo helpers caso queira usar sem a classe Camera.

inline glm::mat4 viewLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up){
    return glm::lookAt(eye, center, up);
}

inline glm::mat4 perspectiveProj(float fovYdeg, float aspect, float nearp, float farp){
    return glm::perspective(glm::radians(fovYdeg), aspect, nearp, farp);
}

inline glm::mat4 orthoProj(float l,float r,float b,float t,float n,float f){
    return glm::ortho(l,r,b,t,n,f);
}

// MVP direto
inline glm::mat4 mvp(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    return P * V * M;
}
