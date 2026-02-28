#pragma once

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
};
