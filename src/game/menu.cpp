#include "game/menu.h"
#include "imgui.h"
#include <glm/ext.hpp>

Menu::Menu(MAI::Renderer *ren, GameSettings &gameSett, Inputs *in)
    : ren(ren), input(in), gameSettings(gameSett) {

  prepareDepthTexs();

  MAI::Shader *vert = ren->createShader(SHADERS_PATH "static_object.vert");
  MAI::Shader *frag = ren->createShader(SHADERS_PATH "static_object.frag");

  pipeline = ren->createPipeline({
      .vert = vert,
      .frag = frag,
      .color =
          {
              .blendEnable = true,
              .srcColorBlend = MAI::BlendFactor::Src_Alpha,
              .dstColorBlend = MAI::BlendFactor::Minus_Src_Alpha,
          },
      .depthFormat = depthTexture->getDeptFormat(),
      .sampleCount = gameSett.getMSAASample(),
  });
  delete vert;
  delete frag;

  plane = Shapes::getPlane(ren);

  logo = ImageLoader::loadFromFile(ren, RESOURCES_PATH "logo.png");
  ready = true;
}

void Menu::prepareDepthTexs() {
  if (depthTexture)
    delete depthTexture;

  if (msaTexture)
    delete msaTexture;

  depthTexture = ren->createImage({
      .type = MAI::TextureType_2D,
      .format = MAI::Format_Z_F32,
      .dimensions = {gameSettings.width, gameSettings.height},
      .usage = MAI::Attachment_Bit,
      .sampleCount = gameSettings.getMSAASample(),
  });

  msaTexture = ren->createImage({
      .type = MAI::TextureType_2D,
      .format = MAI::Format_BGRA_S8,
      .dimensions = {gameSettings.width, gameSettings.height},
      .usage = MAI::Color_Attachment_Bit,
      .sampleCount = gameSettings.getMSAASample(),
  });
}

void Menu::mainMenu() {
  int width, height;
  input->getWindowSize(width, height);
  if (!width || !height)
    return;

  if (width != gameSettings.width || height != gameSettings.height) {
    gameSettings.width = width;
    gameSettings.height = height;
    ren->getVulkanContext()->recreateSwapChain();
    ren->resetDepth = true;
    return;
  }

  if (ren->resetDepth) {
    prepareDepthTexs();
  }

  MAI::CommandBuffer *buff = ren->acquireCommandBuffer();
  buff->cmdBeginRendering({
      .texture = depthTexture,
      .colorTexture = msaTexture,
  });
  buff->cmdEndRendering();
  ren->submit();
  delete buff;
}

Menu::~Menu() {
  delete logo;
  delete pipeline;
  delete depthTexture;
  delete msaTexture;
  Shapes::deleteShape(plane);
}
