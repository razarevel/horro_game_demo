#pragma once
#include "engine/mai_config.h"
#include "engine/mai_vk.h"

#include <glm/glm.hpp>

struct Debugger {
  Debugger(MAI::Renderer *ren, VkFormat format);
  ~Debugger();

  void drawGrid(MAI::CommandBuffer *buff, glm::mat4 pv, glm::vec3 cameraPos);

private:
  MAI::Pipeline *pipeline;
};
