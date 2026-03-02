#pragma once
#include "engine/mai_config.h"
#include "engine/mai_vk.h"
#include "engine/stbi_image.h"
#include <cassert>
#include <iostream>
#include <string>

struct ImageLoader {
  static MAI::Texture *loadFromFile(MAI::Renderer *ren, std::string path) {
    int w, h, comp;
    const stbi_uc *pixels = stbi_load(path.c_str(), &w, &h, nullptr, 4);
    if (!pixels) {
      std::cerr << "failed to load texture at: " << path << std::endl;
      return nullptr;
    }

    assert(pixels);

    MAI::Texture *tex = ren->createImage({
        .type = MAI::TextureType_2D,
        .format = MAI::Format_RGBA_S8,
        .dimensions = {(uint32_t)w, (uint32_t)h},
        .data = pixels,
        .usage = MAI::Sampled_Bit,
    });

    stbi_image_free((void *)pixels);

    return tex;
  }
};
