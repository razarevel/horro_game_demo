#include "game/menu.h"
#include "GLMHelper.h"
#include "imgui.h"
#include <algorithm>
#include <cstdint>
#include <glm/ext.hpp>

Menu::Menu(MAI::Renderer *ren, GLFWwindow *window, GameSettings &gameSett,
           Inputs *in)
    : ren(ren), input(in), gameSettings(gameSett) {

  camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
  debugCam = new Camera(glm::vec3(0.0f, 2.0f, 3.0f));

  prepareDepthTexs();
  perparePipelines();

  VkFormat format = depthTexture->getDeptFormat();
  debugger = new Debugger(ren, format);

  imgui = new ImGuiRenderer(ren, window, format);
  fnr = new FontRenderer(ren, format, gameSettings);

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
  if (gameMode != 1)
    return;

  const double newTimeStamp = glfwGetTime();
  deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
  timeStamp = newTimeStamp;

  debugCam->ProcessKeyboard(deltaSeconds, *input);
  debugCam->ProcessMouseMovement(*input);
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
    // printf("%d, %d, %d\n", mouseX, mouseY, id);
  }
}

void Menu::drawMenus(MAI::CommandBuffer *buff, glm::mat4 pv,
                     glm::vec3 cameraPos, bool offscreen) {
  if (offscreen)
    buff->bindPipeline(pipelinePicking);
  else
    buff->bindPipeline(pipeline);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  struct PushConstant {
    glm::mat4 mvp;
    uint32_t texId;
    uint32_t id;
    uint64_t vertices;
  } pc{
      .mvp = pv * model,
      .texId = logo->getIndex(),
      .id = 10,
      .vertices = ren->gpuAddress(plane.vertBuffer),
  };

  buff->cmdPushConstant(&pc);
  buff->bindIndexBuffer(plane.indexBuffer, 0, MAI::Uint16);
  buff->cmdDrawIndex(plane.indicesSize);

  for (auto &md : menuOptions) {
    model = glm::mat4(1.0f);
    model = GLMHelper::toMat4(md.pos, md.rotate, md.scale);
  }
}

glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
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
  glm::mat4 p = glm::perspective(glm::radians(60.0f), ratio, .01f, 10.0f);
  p[1][1] *= -1;
  glm::mat4 debugProj =
      glm::perspective(glm::radians(45.0f), ratio, .01f, 1000.0f);
  debugProj[1][1] *= -1;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = camera->GetViewMatrix();
  glm::mat4 debugView = debugCam->GetViewMatrix();
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  glm::mat4 mvp;
  if (gameMode)
    mvp = debugProj * debugView * model;
  else
    mvp = p * view * model;

  MAI::CommandBuffer *buff = ren->acquireCommandBuffer();

  fnr->setText("Yokoso watashi no soul society", glm::vec2(0.5f, 0.5f));

  // offscreen
  buff->cmdBeginRendering({
      .depth = {.texture = depthTexture},
      .color =
          {
              {.image = colorTexture->getImage(),
               .imageView = colorTexture->getImageView()},
          },
  });
  {

    if (gameMode == 0) {
      drawMenus(buff, p * view, camera->Position, true);
    } else if (gameMode) {
      drawMenus(buff, debugProj * debugView, debugCam->Position, true);
    }
  }

  buff->cmdEndRendering(colorTexture, false);
  colorPicking();

  // rendering
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

    // if (gameMode == 0) {
    //   drawMenus(buff, p * view, camera->Position);
    // } else if (gameMode) {
    //   drawMenus(buff, debugProj * debugView, debugCam->Position);
    // }

    if (gameMode == 0)
      fnr->draw(buff, p * view);
    else if (gameMode)
      fnr->draw(buff, debugProj * debugView);
  }
  buff->cmdEndRendering(nullptr, false);

  // ui
  buff->cmdBeginRendering({
      .depth =
          {
              .texture = depthTexture,
          },
      .color = {{
          .image = ren->getSwapChainImage(),
          .imageView = ren->getSwapChainImageView(),
          .load = MAI::LoadOp_Load,
          .store = MAI::StoreOp_Store,
      }},
  });

  if (gameMode == 1)
    debugger->drawGrid(buff, debugProj * debugView, debugCam->Position);

  // imgui
  imgui->beginFrame({gameSettings.width, gameSettings.height});
  renderGui();

  // canvas
  // if (gameMode == 1) {
  //   canva3D.setMatrix(debugProj * debugView);
  //   canva3D.frustum(view, p, glm::vec4(1.0f));
  //   canva3D.render(ren, depthTexture->getDeptFormat(), buff);
  // }

  imgui->endFrame(buff);

  buff->cmdEndRendering();
  ren->submit();
  delete buff;
}

void Menu::renderGui() {
  if (const ImGuiViewport *v = ImGui::GetMainViewport()) {
    ImGui::SetNextWindowPos({v->WorkPos.x, v->WorkPos.y}, ImGuiCond_Always,
                            {0.0f, 0.0f});
    ImGui::SetNextWindowSize({v->WorkSize.x * 0.3f, v->WorkSize.y},
                             ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.50f);
  }
  ImGui::Begin("Viewport");

  ImGui::Text("Game Mode");
  ImGui::RadioButton("Player", &gameMode, 0);
  ImGui::SameLine();
  ImGui::RadioButton("Debugger", &gameMode, 1);
  ImGui::InputFloat3("Position", glm::value_ptr(pos));

  ImGui::End();
}

Menu::~Menu() {
  canva3D.freeResoucs();
  delete imgui;
  delete debugger;
  delete logo;
  delete pipeline;
  delete pipelinePicking;
  delete colorTexture;
  delete depthTexture;
  delete depthTextureMSAA;
  delete msaTexture;
  delete camera;
  delete fnr;
  Shapes::deleteShape(plane);
}
