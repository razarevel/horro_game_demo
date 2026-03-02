#include "engine/assets.h"
#include "engine/model.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <mutex>
#include <thread>

namespace fs = std::filesystem;

namespace assets {
std::mutex mtx;
uint32_t id = 1;
}; // namespace assets

void loadAssets(MAI::Renderer *ren, std::string str,
                std::vector<ModelInfo> &models);

Assets::Assets(MAI::Renderer *ren) {
  std::string dir = RESOURCES_PATH "assets";
  std::vector<std::string> assets;
  for (const auto &entry : fs::directory_iterator(dir)) {
    std::string str = entry.path();
    assets.emplace_back(str);
  }

  std::vector<std::jthread> j;
  j.reserve(assets.size());
  models.reserve(assets.size());

  for (auto &str : assets)
    j.emplace_back(loadAssets, ren, str, std::ref(models));
}

void loadAssets(MAI::Renderer *ren, std::string str,
                std::vector<ModelInfo> &models) {
  std::string path;
  std::string texPath;
  for (const auto &entry : fs::directory_iterator(str)) {
    std::string str = entry.path();
    if (str.find(".gltf") != std::string::npos)
      path = str;
    if (str.find("0.png") != std::string::npos)
      texPath = str;
  }

  MAI::Texture *tex = ImageLoader::loadFromFile(ren, texPath);
  Model *md = new Model(ren, path);

  std::lock_guard<std::mutex> lock(assets::mtx);
  models.emplace_back(ModelInfo{
      .id = assets::id,
      .name = str.substr(str.find_last_of('/') + 1),
      .model = md,
      .identifier = tex,
  });
  assets::id++;
}

void Assets::assetsGui(uint32_t &id, bool &addAsset) {
  ImGui::NewLine();
  if (ImGui::TreeNode("Assets")) {
    if (ImGui::BeginTable("Texture Column", 4)) {
      for (auto &md : models) {
        ImGui::TableNextColumn();
        if (ImGui::ImageButton(md.name.c_str(), md.identifier->getIndex(),
                               ImVec2(70, 70))) {
          id = md.id;
          addAsset = true;
        }
      }
      ImGui::EndTable();
    }
    ImGui::TreePop();
  }
}

Assets::~Assets() {
  for (auto &md : models) {
    delete md.model;
    delete md.identifier;
  }
}
