#include "game/menu.h"
#include "imgui.h"
#include <algorithm>
#include <cstdint>
#include <glm/ext.hpp>

Menu::Menu(MAI::Renderer *ren, GameSettings &gameSett, Inputs *in)
    : ren(ren), input(in), gameSettings(gameSett) {

  camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

  prepareDepthTexs();
  perparePipelines();

  plane = Shapes::getPlane(ren);
  logo = ImageLoader::loadFromFile(ren, RESOURCES_PATH "logo.png");
  ready = true;
}

void Menu::perparePipelines() {
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
      .sampleCount = gameSettings.getMSAASample(),
  });
  delete frag;

  frag = ren->createShader(SHADERS_PATH "picking.frag");
  pipelinePicking = ren->createPipeline({
      .vert = vert,
      .frag = frag,
      .depthFormat = depthTexture->getDeptFormat(),
      .colorFormat = colorTexture->getDeptFormat(),
  });
  delete vert;
  delete frag;
}

void Menu::prepareDepthTexs() {
  if (depthTexture)
    delete depthTexture;
  if (depthTextureMSAA)
    delete depthTextureMSAA;

  if (msaTexture)
    delete msaTexture;

  if (msaTexture)
    delete msaTexture;

  depthTextureMSAA = ren->createImage({
      .type = MAI::TextureType_2D,
      .format = MAI::Format_Z_F32,
      .dimensions = {gameSettings.width, gameSettings.height},
      .usage = MAI::Attachment_Bit,
      .sampleCount = gameSettings.getMSAASample(),

  });

  depthTexture = ren->createImage({
      .type = MAI::TextureType_2D,
      .format = MAI::Format_Z_F32,
      .dimensions = {gameSettings.width, gameSettings.height},
      .usage = MAI::Attachment_Bit,
  });

  msaTexture = ren->createImage({
      .type = MAI::TextureType_2D,
      .format = MAI::Format_BGRA_S8,
      .dimensions = {gameSettings.width, gameSettings.height},
      .usage = MAI::Color_Attachment_Bit,
      .sampleCount = gameSettings.getMSAASample(),
  });

  colorTexture = ren->createImage({
      .type = MAI::TextureType_2D,
      .format = MAI::Format_R_Uint32,
      .dimensions = {gameSettings.width, gameSettings.height},
      .usage = MAI::Color_Attachment_Bit,
      .srcOptimal = true,
  });
}

float deltaSeconds = 0.0f;
double timeStamp = glfwGetTime();
void Menu::updateUtils() {
  const double newTimeStamp = glfwGetTime();
  deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
  timeStamp = newTimeStamp;

  camera->ProcessKeyboard(deltaSeconds, *input);
  camera->ProcessMouseMovement(*input);
}

void Menu::colorPicking() {
  if (!input->mouseKeys[0])
    return;

  double xpos, ypos;
  input->getMousePos(xpos, ypos);
  int mouseX = static_cast<int>(xpos);
  int mouseY = static_cast<int>(ypos);

  mouseX = std::clamp(mouseX, 0, (int)gameSettings.width - 1);
  mouseY = std::clamp(mouseY, 0, (int)gameSettings.height - 1);

  if (mouseX > 0 && mouseY > 0) {
    int id = ren->imagePicking(mouseX, mouseY, colorTexture);
    printf("%d, %d, %d\n", mouseX, mouseY, id);
  }
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

  updateUtils();

  float ratio = width / (float)height;
  glm::mat4 p = glm::perspective(glm::radians(45.0f), ratio, 1.0f, 1000.0f);
  p[1][1] *= -1;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = camera->GetViewMatrix();
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  struct PushConstant {
    glm::mat4 mvp;
    uint32_t texId;
    uint32_t id;
    uint64_t vertices;
  } pc{
      .mvp = p * view * model,
      .texId = logo->getIndex(),
      .id = 10,
      .vertices = ren->gpuAddress(plane.vertBuffer),
  };

  MAI::CommandBuffer *buff = ren->acquireCommandBuffer();
  buff->cmdBeginRendering({
      .depth = {.texture = depthTexture},
      .color =
          {
              {.image = colorTexture->getImage(),
               .imageView = colorTexture->getImageView()},
          },
  });
  {
    buff->bindPipeline(pipelinePicking);
    buff->cmdPushConstant(&pc);
    buff->bindIndexBuffer(plane.indexBuffer, 0, MAI::Uint16);
    buff->cmdDrawIndex(plane.indicesSize);
  }
  buff->cmdEndRendering(colorTexture);
  colorPicking();

  buff->cmdBeginRendering({
      .depth = {.texture = depthTextureMSAA},
      .color = {{
          .image = msaTexture->getImage(),
          .imageView = msaTexture->getImageView(),
          .resolveImage = ren->getSwapChainImage(),
          .resolveView = ren->getSwapChainImageView(),
      }},
  });
  {
    buff->bindPipeline(pipeline);
    buff->cmdPushConstant(&pc);
    buff->bindIndexBuffer(plane.indexBuffer, 0, MAI::Uint16);
    buff->cmdDrawIndex(plane.indicesSize);
  }
  buff->cmdEndRendering();
  ren->submit();
  delete buff;
}

Menu::~Menu() {
  delete logo;
  delete pipeline;
  delete pipelinePicking;
  delete colorTexture;
  delete depthTexture;
  delete depthTextureMSAA;
  delete msaTexture;
  delete camera;
  Shapes::deleteShape(plane);
}
