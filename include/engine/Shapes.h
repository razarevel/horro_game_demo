#pragma once
#include "engine/mai_config.h"
#include "engine/mai_vk.h"

#include <glm/ext.hpp>
#include <glm/glm.hpp>

struct Shape {
  MAI::Buffer *vertBuffer;
  MAI::Buffer *indexBuffer;
  uint32_t indicesSize;
};

struct Shapes {
  static Shape getCube(MAI::Renderer *ren, bool onlyPos = false);
  static Shape getSphere(MAI::Renderer *ren);
  static Shape getCylinder(MAI::Renderer *ren, float radius, float height,
                           int segments);
  static Shape getCone(MAI::Renderer *ren, float radius, float height,
                       int segment);

  static void deleteShape(Shape &shape);
};
