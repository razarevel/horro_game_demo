#pragma once
#include "GLMHelper.h"
#include "engine/Shapes.h"
#include "engine/textures.h"

#include <glm/glm.hpp>

enum ObjectType {
  SHAPE,
  ASSET,
};

struct ObjectPerFrame {
  glm::mat4 model;
  glm::vec4 color;
  float tiling;
  float ao;
  float metallic;
  float roughness;
  uint32_t albedoTex;
  uint32_t normTex;
  uint32_t armTex;
  uint32_t padding;
};

struct ObjectsInfo {
  uint32_t id;
  ObjectType type;
  uint32_t object_id;
  uint32_t tex_id;
  float tiling;
  float ao;
  float metallic;
  float roughness;
  glm::vec3 color;
  glm::vec3 pos;
  glm::vec3 rotate;
  glm::vec3 scale;
};

struct Objects {
  Objects(MAI::Renderer *ren, VkFormat format, Textures *texs);
  ~Objects();

  void draw(MAI::CommandBuffer *buff, uint64_t buffPerFrame, glm::mat4 pv);
  void addObject(ObjectsInfo info);
  void addObjects(std::vector<ObjectsInfo> &infos);

  void allObjectGui();
  void objectGui();

  std::vector<ObjectsInfo> &getObjectsInfos() { return objects; }

private:
  MAI::Renderer *ren;
  MAI::Pipeline *pipeline;
  std::vector<ObjectsInfo> objects;
  Shape cube;
  MAI::Buffer *objectBuff = nullptr;
  Textures *textures = nullptr;
  int focuseId = -1;

  bool updateBuff = false;

  void settingBuffers();
};
