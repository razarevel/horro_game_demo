#pragma once
#include "engine/mai_config.h"
#include "engine/mai_vk.h"
#include "engine/utilsMath.h"

#include <glm/glm.hpp>

struct LineCanvas2D {
  void clear() { lines_.clear(); }
  void line(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec4 &c) {
    lines_.push_back({.p1 = p1, .p2 = p2, .color = c});
  };
  void render(const char *nameImGuiWindow);

private:
  struct LineData {
    glm::vec2 p1, p2;
    glm::vec4 color;
  };
  std::vector<LineData> lines_;
};

struct LineCanvas3D {
  void clear() { lines_.clear(); }
  void line(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec4 &c);
  void plane(const glm::vec3 &o, const glm::vec3 &v1, const glm::vec3 &v2,
             int n1, int n2, float s1, float s2, const glm::vec4 &color,
             const glm::vec4 &outlineColor);
  void box(const mat4 &m, const BoundingBox &box, const vec4 &c);
  void box(const mat4 &m, const vec3 &size, const vec4 &c);
  void frustum(const mat4 &camView, const mat4 &camProj, const vec4 &color);

  void setMatrix(const mat4 &mvp) { mvp_ = mvp; }
  void render(MAI::Renderer *ren, VkFormat format, MAI::CommandBuffer *buff);

  void freeResoucs();

private:
  glm::mat4 mvp_ = glm::mat4(1.0f);

  struct LineData {
    glm::vec4 pos;
    glm::vec4 color;
  };

  std::vector<LineData> lines_;

  MAI::Pipeline *pipeline = nullptr;
  MAI::Buffer *linesBuffer_ = nullptr;
};
