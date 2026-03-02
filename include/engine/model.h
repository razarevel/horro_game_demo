#pragma once
#include "GLMHelper.h"
#include "engine/imageLoader.h"
#include "mai_config.h"
#include "mai_vk.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>

struct MeshInfo {
  MAI::Buffer *verBuffer;
  MAI::Buffer *indexBuffer;
  uint32_t indicesSize;
  uint32_t tex_id;
};

struct NodeInfo {
  std::vector<MeshInfo> meshes;
  glm::mat4 localTransform;
};

struct MeshTextureInfo {
  uint32_t id;
  glm::vec4 baseColor;

  MAI::Texture *diff = nullptr;
  glm::vec4 diffColor;

  MAI::Texture *norm = nullptr;

  MAI::Texture *emissive = nullptr;
  glm::vec4 emissiveColor;

  MAI::Texture *arm = nullptr;
  glm::vec4 armColor;
};

struct Model {
  Model(MAI::Renderer *ren, std::string path);
  ~Model();

  std::vector<NodeInfo> &getNodes() { return nodes; }
  std::vector<MeshTextureInfo> &getTextures() { return textures; }

private:
  MAI::Renderer *ren;
  std::string dir;
  std::vector<NodeInfo> nodes;
  std::vector<std::string> loadTextures;
  std::vector<MeshTextureInfo> textures;

  void processNode(const aiNode *node, const aiScene *scene,
                   glm::mat4 parentTrans);
  MeshInfo processMesh(const aiMesh *mesh, const aiScene *scene);

  MAI::Texture *loadTexture(const char *filename);
  void prepareTextures(const aiMaterial *mat, uint32_t id);
};
