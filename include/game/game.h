#pragma once
#include "engine/assets.h"
#include "engine/maiApp.h"
#include "engine/scene/objects.h"
#include "engine/scene/static_models.h"
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
  Assets *assets;
  Objects *objects = nullptr;
  StaticModels *staticModels = nullptr;
  MAI::Buffer *buffPerFrame;

  void initGame();
  void guiWidget();
  void clearAll();
  void reset();
};
