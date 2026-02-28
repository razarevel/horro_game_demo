#include "engine/textures.h"
#include "engine/stbi_image.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <imgui.h>
#include <iostream>
#include <ktx.h>
#include <thread>

namespace fs = std::filesystem;

namespace texture {
uint32_t count = 0;
std::mutex mtx;
}; // namespace texture

std::vector<std::string> cacheTextureFiles;

void loadTextures(MAI::Renderer *ren, std::string dir,
                  std::vector<Texture> &textures);

Textures::Textures(MAI::Renderer *ren) {
  std::string texDir = RESOURCES_PATH "textures";
  std::string cacheDir = RESOURCES_PATH "textures/cache";

  for (const auto &entry : fs::directory_iterator(cacheDir)) {
    cacheTextureFiles.emplace_back(entry.path());
  }

  std::vector<std::jthread> threads;
  for (const auto &entry : fs::directory_iterator(texDir)) {
    std::string str = entry.path();
    if (str.find("cache") == std::string::npos)
      threads.emplace_back(loadTextures, ren, str, std::ref(textures));
  }
}

std::string getFilename(std::string filename) {
  std::string name = filename;
  std::replace(name.begin(), name.end(), '\\', '/');
  name = name.substr(name.find_last_of('/') + 1);
  name = name.substr(0, name.find_last_of('.'));
  return name;
}

void loadTextures(MAI::Renderer *ren, std::string dir,
                  std::vector<Texture> &textures) {

  std::string pathName = dir;
  std::replace(pathName.begin(), pathName.end(), '\\', '/');
  pathName = pathName.substr(pathName.find_last_of('/') + 1);

  Texture tex;
  tex.name = pathName;

  for (const auto &entry : fs::directory_iterator(dir)) {
    std::string str = entry.path();

    std::string name = getFilename(str);

    bool isPreset = false;
    std::string pathToKtx;
    if (!cacheTextureFiles.empty())
      for (auto file : cacheTextureFiles)
        if (getFilename(file) == name) {
          isPreset = true;
          pathToKtx = file;
        }

    ktxTexture1 *ktxTex = nullptr;
    if (!isPreset) {
      int w, h, compj;
      const stbi_uc *pixels = stbi_load(str.c_str(), &w, &h, nullptr, 4);
      if (!pixels) {
        std::cerr << "failed to load textures: " << pathName << std::endl;
        std::cerr << "path: " << dir << std::endl;
        assert(false);
      }

      ktxTextureCreateInfo createInfo = {
          .glInternalformat = GL_RGBA8,
          .vkFormat = VK_FORMAT_R8G8B8A8_SRGB,
          .baseWidth = (uint32_t)w,
          .baseHeight = (uint32_t)h,
          .baseDepth = 1,
          .numDimensions = 2,
          .numLevels = 1,
          .numLayers = 1,
          .numFaces = 1,
          .generateMipmaps = VK_FALSE,
      };
      if (ktxTexture1_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE,
                             &ktxTex) != KTX_SUCCESS)
        assert(false);
      size_t kBufferSize = w * h * 4;
      memcpy(ktxTex->pData, (void *)pixels, kBufferSize);
      stbi_image_free((void *)pixels);

      name += ".ktx";
      std::string cacheDir = RESOURCES_PATH "textures/cache/" + name;
      ktxTexture_WriteToNamedFile(ktxTexture(ktxTex), cacheDir.c_str());
    } else {
      ktxTexture1_CreateFromNamedFile(
          pathToKtx.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex);
    }

    MAI::Texture *texture = ren->createImage({
        .type = MAI::TextureType_2D,
        .format = MAI::Format_RGBA_S8,
        .dimensions = {ktxTex->baseWidth, ktxTex->baseHeight},
        .data = ktxTex->pData,
        .usage = MAI::Sampled_Bit,
    });

    if (str.find("diff") != std::string::npos)
      tex.albedo = texture;
    else if (str.find("nor") != std::string::npos)
      tex.normal = texture;
    else if (str.find("disp") != std::string::npos)
      tex.disp = texture;
    else if (str.find("arm") != std::string::npos)
      tex.arm = texture;
    else {
      std::cerr << "unsupported type" << std::endl;
      delete texture;
    }

    ktxTexture_Destroy(ktxTexture(ktxTex));
  }

  std::lock_guard<std::mutex> lock(texture::mtx);
  tex.id = texture::count;
  textures.emplace_back(tex);
  texture::count++;
}

void Textures::guiWidget(uint32_t &id) {
  ImGui::NewLine();
  ImGui::Text("Textures");
  if (ImGui::BeginTable("Texture Column", 3)) {
    for (auto &tex : textures) {
      ImGui::TableNextColumn();
      if (ImGui::ImageButton(tex.name.c_str(), tex.albedo->getIndex(),
                             ImVec2(100, 100)))
        id = tex.id;
    }
    ImGui::EndTable();
  }
}

Textures::~Textures() {
  for (auto tex : textures) {
    if (tex.albedo)
      delete tex.albedo;
    if (tex.normal)
      delete tex.normal;
    if (tex.disp)
      delete tex.disp;
    if (tex.arm)
      delete tex.arm;
  }
}
