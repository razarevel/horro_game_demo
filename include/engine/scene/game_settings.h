#pragma once
#include "engine/json.hpp"
#include "engine/mai_vk.h"
#include <string>

struct GameSettings {
  std::string name = "Purge Of Black Shadow";
  uint32_t width = 1200;
  uint32_t height = 800;
  bool isFullScreen = false;
  uint32_t graphics_mode = 0;
  uint32_t msaa = 1;
  void init();
  void guiSettings();
  void save();

  MAI::MSAASample getMSAASample();
};
