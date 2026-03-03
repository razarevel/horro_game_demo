#pragma once
#include "engine/assets.h"
#include <glm/glm.hpp>

struct StaticModelInfo {
  uint32_t id;
  ModelInfo *model;
  glm::vec3 pos = glm::vec3(0.0f);
  glm::vec3 rotate = glm::vec3(0.0f);
  float scale = 1.0f;
};

struct StaticModels {
  StaticModels(MAI::Renderer *ren, VkFormat format, Assets *assets,
               MAI::MSAASample count = MAI::Count_1_Bit);
  ~StaticModels();

  void addmodel(StaticModelInfo info);
  void draw(MAI::CommandBuffer *buff, glm::mat4 pv, uint64_t buffAddress,
            glm::vec3 cameraPos);

  void allModelGui();
  void modelGui();

private:
  MAI::Renderer *ren;
  Assets *assets;
  MAI::Pipeline *pipeline;
  std::vector<StaticModelInfo> models;
  bool update = false;
  int focusId = -1;
};
