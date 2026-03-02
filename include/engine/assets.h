#pragma once
#include "model.h"

struct ModelInfo {
  uint32_t id;
  std::string name;
  Model *model;
  MAI::Texture *identifier;
};

struct Assets {
  Assets(MAI::Renderer *ren);
  ~Assets();

  void assetsGui(uint32_t &id, bool &addAsset);

  ModelInfo *getModelInfo(uint32_t id) {
    for (auto &md : models)
      if (md.id == id)
        return &md;
    assert(false);
  };

private:
  std::vector<ModelInfo> models;
};
