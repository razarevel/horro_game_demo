#pragma once
#include "engine/Camera.h"
#include "engine/Shapes.h"
#include "engine/imageLoader.h"
#include "engine/inputs.h"
#include "engine/mai_config.h"
#include "engine/mai_vk.h"
#include "engine/scene/game_settings.h"
#include "game/debugger.h"

struct Menu {
  Menu(MAI::Renderer *ren, GameSettings &gameSett, Inputs *in);
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

  void prepareDepthTexs();
  void updateUtils();

  void perparePipelines();
  void colorPicking();
};
