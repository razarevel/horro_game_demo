#include "game/debugger.h"

Debugger::Debugger(MAI::Renderer *ren, VkFormat format) {
  MAI::Shader *vert = ren->createShader(SHADERS_PATH "spvs/grid.vspv");
  MAI::Shader *frag = ren->createShader(SHADERS_PATH "spvs/grid.fspv");

  pipeline = ren->createPipeline({
      .vert = vert,
      .frag = frag,
      .color =
          {
              .blendEnable = true,
              .srcColorBlend = MAI::Src_Alpha,
              .dstColorBlend = MAI::Minus_Src_Alpha,
          },
      .depthFormat = format,
  });
  delete vert;
  delete frag;
}

void Debugger::drawGrid(MAI::CommandBuffer *buff, glm::mat4 pv,
                        glm::vec3 cameraPos) {
  struct PushConstant {
    glm::mat4 mvp;
    glm::vec4 cameraPos;
    glm::vec4 origin;
  } pc{
      .mvp = pv,
      .cameraPos = glm::vec4(cameraPos, 1.0f),
      .origin = glm::vec4(0.0f),
  };
  buff->bindPipeline(pipeline);
  buff->cmdPushConstant(&pc);
  buff->cmdDraw(6);
}

Debugger::~Debugger() { delete pipeline; }
