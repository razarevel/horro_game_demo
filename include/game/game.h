#pragma once
#include "engine/maiApp.h"
#include "engine/scene/objects.h"
#include "engine/textures.h"

struct Game {
  Game();
  ~Game();
  void run();
  void load();
  void save();

private:
  MaiApp *app;
  MAI::Renderer *ren;

  // resources
  Textures *textures;
  Objects *objects = nullptr;
  MAI::Buffer *buffPerFrame;

  void initGame();
  void guiWidget();
  void clearAll();
  void reset();
};
