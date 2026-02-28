#pragma once

#include "engine/mai_config.h"
#include "engine/mai_vk.h"
#include <string>

struct Texture {
  uint32_t id;
  std::string name;
  MAI::Texture *albedo = nullptr;
  MAI::Texture *normal = nullptr;
  MAI::Texture *disp = nullptr;
  MAI::Texture *arm = nullptr;
};

struct Textures {
  Textures(MAI::Renderer *ren);
  ~Textures();

  Texture *getTextures(uint32_t id) {
    for (auto &tex : textures)
      if (tex.id == id)
        return &tex;
    return nullptr;
  }

  auto &getAllTextures() const { return textures; }

  void guiWidget(uint32_t &id);

private:
  std::vector<Texture> textures;
};
