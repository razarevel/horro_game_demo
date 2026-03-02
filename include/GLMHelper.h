#pragma once

#include <assimp/postprocess.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

struct GLMHelper {
  static glm::mat4 toMat4(glm::vec3 pos, glm::vec3 r, glm::vec3 s) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, r.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, r.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, r.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, s);
    return model;
  }

  static glm::mat4 toMat4(glm::vec3 pos, glm::vec3 r, float s) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, r.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, r.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, r.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(s));
    return model;
  }

  static glm::mat4 ConvertMatrix(const aiMatrix4x4 &m) {
    glm::mat4 model = glm::mat4(m.a1, m.a2, m.a3, m.a4, m.b1, m.b2, m.b3, m.b4,
                                m.c1, m.c2, m.c3, m.c4, m.d1, m.d2, m.d3, m.d4);
    return glm::transpose(model);
  }
};
