#pragma once
#include "engine/inputs.h"
#include "engine/mai_config.h"
#include "engine/mai_vk.h"
#include "engine/scene/game_settings.h"
#include "game/menu.h"

struct Game {
  Game();
  ~Game();
  void run();
  void save();

private:
  GLFWwindow *window;
  MAI::Renderer *ren;

  Menu *menu;
  GameSettings gameSettings;
  Inputs *inputs;

  void load();
  bool blackScreen();
};
