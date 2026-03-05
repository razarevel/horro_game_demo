#include "engine/maiApp.h"
#include "imgui.h"
#include <iostream>

MouseState mouseState;

void setKeyboardConfig(GLFWwindow *window, int key, int scancode, int action,
                       int mods);
void processInput(GLFWwindow *window);

// float deltaSeconds = 0.0f;
// bool firstMouse = true;
// float lastX = 0.0f;
// float lastY = 0.0f;

MaiApp::MaiApp(MAI::WindowInfo &info, MAI::MSAASample count)
    : sampleCount(count) {
  camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
  window = MAI::initWindow(info);

  windowSize = {
      .width = info.width,
      .height = info.height,
  };

  ren = MAI::initVulkanWithSwapChain(window, info.appName);

  prepareTextures();

  setMouseConfig();
  glfwSetKeyCallback(window, setKeyboardConfig);
  glfwSetWindowUserPointer(window, this);

  imgui = new ImGuiRenderer(ren, window, depthTexture->getDeptFormat(),
                            sampleCount);
}

void MaiApp::setMouseConfig() {
  glfwSetCursorPosCallback(window, [](auto *window, double x, double y) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    mouseState.pos.x = static_cast<float>(x / width);
    mouseState.pos.y = 1.0f - static_cast<float>(y / height);
  });

  glfwSetMouseButtonCallback(
      window, [](auto *window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
          mouseState.pressedLeft = action == GLFW_PRESS;
        }
      });
}

bool undoMods[2];
void setKeyboardConfig(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {

  const bool pressed = action != GLFW_RELEASE;
  if (key == GLFW_KEY_F4 && pressed && mods & GLFW_MOD_ALT)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (key == GLFW_KEY_Z && pressed && mods & GLFW_MOD_CONTROL)
    undoMods[0] = true;
  if (key == GLFW_KEY_Y && pressed && mods & GLFW_MOD_CONTROL)
    undoMods[1] = true;
}

void processInput(GLFWwindow *window) {
  MaiApp *mai = reinterpret_cast<MaiApp *>(glfwGetWindowUserPointer(window));

  // if (glfwGetKey(window, GLFW_KEY_W) && GLFW_PRESS)
  //   mai->camera->ProcessKeyboard(FORWARD, deltaSecond);
  // if (glfwGetKey(window, GLFW_KEY_S) && GLFW_PRESS)
  //   mai->camera->ProcessKeyboard(BACKWARD, deltaSecond);
  // if (glfwGetKey(window, GLFW_KEY_A) && GLFW_PRESS)
  //   mai->camera->ProcessKeyboard(LEFT, deltaSecond);
  // if (glfwGetKey(window, GLFW_KEY_D) && GLFW_PRESS)
  //   mai->camera->ProcessKeyboard(RIGHT, deltaSecond);
}

std::array<bool, 2> MaiApp::getMods() {
  std::array<bool, 2> mods;
  mods[0] = undoMods[0];
  mods[1] = undoMods[1];
  return mods;
}

void MaiApp::updateMouseMovement() {
  if (ImGui::GetIO().WantCaptureMouse)
    return;

  // if (mouseState.pressedLeft) {
  //   float xpos = static_cast<float>(mouseState.pos.x);
  //   float ypos = static_cast<float>(mouseState.pos.y);
  //
  //   if (firstMouse) {
  //     lastX = xpos;
  //     lastY = ypos;
  //     firstMouse = false;
  //   }
  //
  //   float xoffset = xpos - lastX;
  //   float yoffset = lastY - ypos;
  //
  //   lastX = xpos;
  //   lastY = ypos;
  //
  //   // camera->ProcessMouseMovement(xoffset, yoffset);
  // } else
  //   firstMouse = true;
}

void MaiApp::prepareTextures() {
  if (depthTexture)
    delete depthTexture;
  if (colorTexture)
    delete colorTexture;

  depthTexture = ren->createImage({
      .type = MAI::TextureType_2D,
      .format = MAI::Format_Z_F32,
      .dimensions = {windowSize.width, windowSize.height},
      .usage = MAI::Attachment_Bit,
      .sampleCount = sampleCount,
  });

  if (sampleCount != MAI::Count_1_Bit)
    colorTexture = ren->createImage({
        .type = MAI::TextureType_2D,
        .format = MAI::Format_BGRA_S8,
        .dimensions = {windowSize.width, windowSize.height},
        .usage = MAI::Color_Attachment_Bit,
        .sampleCount = sampleCount,
    });
}

void MaiApp::run(DrawFrameFunc drawFrame) {
  double timeStamp = glfwGetTime();
  FPS fps;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (!width || !height)
      continue;

    if (windowSize.width != width || windowSize.height != height) {
      windowSize = {(uint32_t)width, (uint32_t)height};
      ren->getVulkanContext()->recreateSwapChain();
      ren->resetDepth = true;
      continue;
    }

    if (ren->resetDepth) {

      glfwGetFramebufferSize(window, &width, &height);

      windowSize = {
          .width = (uint32_t)width,
          .height = (uint32_t)height,
      };
      prepareTextures();
    }

    float ratio = width / (float)height;
    const double newTimeStamp = glfwGetTime();
    // deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
    timeStamp = newTimeStamp;

    processInput(window);
    mouse_state = mouseState;
    updateMouseMovement();

    // fps.tick(deltaSeconds);
    currentFPS = fps.currentFPS_;

    MAI::CommandBuffer *buff = ren->acquireCommandBuffer();

    // beforeDraw(buff, width, height, ratio, deltaSeconds);
    // draw
    buff->cmdBeginRendering({
        // .texture = depthTexture,
        // .colorTexture = colorTexture,
    });
    imgui->beginFrame({(uint32_t)width, (uint32_t)height});
    // drawFrame(buff, width, height, ratio, deltaSeconds);
    // fps
    {
      if (const ImGuiViewport *v = ImGui::GetMainViewport()) {
        ImGui::SetNextWindowPos(
            {v->WorkPos.x + v->WorkSize.x - 15.0f, v->WorkPos.y + 15.0f},
            ImGuiCond_Always, {1.0f, 0.0f});
      }
      ImGui::SetNextWindowBgAlpha(0.30f);
      ImGui::SetNextWindowSize(
          ImVec2(ImGui::CalcTextSize("FPS : _______").x, 0));
      if (ImGui::Begin("##FPS", nullptr,
                       ImGuiWindowFlags_NoDecoration |
                           ImGuiWindowFlags_AlwaysAutoResize |
                           ImGuiWindowFlags_NoSavedSettings |
                           ImGuiWindowFlags_NoFocusOnAppearing |
                           ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove)) {
        ImGui::Text("FPS : %i", (int)currentFPS);
        ImGui::Text("Ms  : %.1f", 1000.0 / currentFPS);
        ImGui::End();
      }
    }
    // ending
    imgui->endFrame(buff);
    buff->cmdEndRendering();
    ren->submit();
    delete buff;
    undoMods[0] = false;
    undoMods[1] = false;
  }
  ren->waitDeviceIdle();
}

MaiApp::~MaiApp() {
  delete imgui;
  delete camera;
  delete colorTexture;
  delete depthTexture;
  glfwDestroyWindow(window);
  glfwTerminate();
  delete ren;
}
