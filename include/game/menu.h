#pragma once
#include "engine/Shapes.h"
#include "engine/imageLoader.h"
#include "engine/inputs.h"
#include "engine/mai_config.h"
#include "engine/mai_vk.h"
#include "engine/scene/game_settings.h"

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

  MAI::Texture *depthTexture = nullptr;
  MAI::Texture *msaTexture = nullptr;

  MAI::Pipeline *pipeline;
  MAI::Texture *logo;
  Shape plane;

  void prepareDepthTexs();
};
