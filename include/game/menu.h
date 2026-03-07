#pragma once
#include "engine/Camera.h"
#include "engine/Shapes.h"
#include "engine/fontRenderer.h"
#include "engine/imageLoader.h"
#include "engine/imguiRenderer.h"
#include "engine/inputs.h"
#include "engine/mai_config.h"
#include "engine/mai_vk.h"
#include "engine/scene/game_settings.h"
#include "engine/scene/lineCanvas.h"
#include "game/debugger.h"

struct MenuOptions {
  glm::vec3 pos;
  glm::vec3 rotate;
  glm::vec3 scale;
  bool mouseOn = false;
};

struct Menu {
  Menu(MAI::Renderer *ren, GLFWwindow *window, GameSettings &gameSett,
       Inputs *in);
  ~Menu();
  void mainMenu();
  void guiMenu();
  bool ready = false;

private:
  Inputs *input;
  MAI::Renderer *ren;
  GameSettings gameSettings;

  // depth textures
  MAI::Texture *depthTextureMSAA = nullptr;
  MAI::Texture *depthTexture = nullptr;
  MAI::Texture *msaTexture = nullptr;
  MAI::Texture *colorTexture = nullptr;

  MAI::Pipeline *pipeline;
  MAI::Pipeline *pipelinePicking;
  MAI::Texture *logo;
  Shape plane;

  Debugger *debugger;
  Camera *camera;
  Camera *debugCam;
  LineCanvas3D canva3D;

  ImGuiRenderer *imgui;
  FontRenderer *fnr;

  int gameMode = 0;

  std::vector<MenuOptions> menuOptions;

  void prepareDepthTexs();
  void updateUtils();
  void perparePipelines();
  void colorPicking();
  void renderGui();

  void drawMenus(MAI::CommandBuffer *buff, glm::mat4 pv, glm::vec3 cameraPos,
                 bool offscreen = false);
};
