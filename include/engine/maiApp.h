#pragma once
#include "engine/Camera.h"
#include "engine/imguiRenderer.h"
#include <array>
#include <functional>

struct MouseState {
  glm::vec3 pos;
  bool pressedLeft = false;
};

using DrawFrameFunc =
    std::function<void(MAI::CommandBuffer *buff, uint32_t width,
                       uint32_t height, float ratio, float deltaSecond)>;
struct FPS {
  FPS(float avgInterval = 0.5f) : avgInterval_(avgInterval) {}
  void tick(float deltaSecond) {
    numFrames++;
    accumulateTime += deltaSecond;
    if (accumulateTime > avgInterval_) {
      currentFPS_ = static_cast<float>(numFrames / accumulateTime);
      numFrames = 0;
      accumulateTime = 0;
    }
  }

public:
  float avgInterval_ = 0.5f;
  uint32_t numFrames = 0;
  double accumulateTime = 0;
  float currentFPS_ = 0.0f;
};

struct MaiApp {
  MaiApp(MAI::WindowInfo &info);
  ~MaiApp();

  VkExtent2D windowSize;
  MAI::Renderer *ren = nullptr;
  GLFWwindow *window = nullptr;
  MAI::Texture *depthTexture = nullptr;
  MAI::Texture *colorTexture = nullptr;
  Camera *camera = nullptr;
  ImGuiRenderer *imgui;
  MouseState mouse_state;
  float currentFPS;
  DrawFrameFunc beforeDraw = [](auto...) {};
  DrawFrameFunc offscreenDraw = [](auto...) {};
  void run(DrawFrameFunc drawFrame);

  static std::array<bool, 2> getMods();

private:
  void setMouseConfig();
  void updateMouseMovement();
};
