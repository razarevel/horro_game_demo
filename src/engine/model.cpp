#include "engine/model.h"
#include "engine/stbi_image.h"
#include <algorithm>
#include <glm/glm.hpp>

#define flags                                                                  \
  aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |                    \
      aiProcess_GenNormals | aiProcess_CalcTangentSpace |                      \
      aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials |    \
      aiProcess_ValidateDataStructure | aiProcess_SortByPType |                \
      aiProcess_FlipUVs

struct MeshVertex {
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec2 uv;
  glm::vec3 tan;
  glm::vec3 bitan;
};

Model::Model(MAI::Renderer *ren, std::string path) : ren(ren) {
  const aiScene *scene = aiImportFile(path.c_str(), flags);

  dir = path;
  std::replace(dir.begin(), dir.end(), '\\', '/');
  dir = dir.substr(0, dir.find_last_of('/') + 1);

  processNode(scene->mRootNode, scene, glm::mat4(1.0f));

  for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
    const aiMaterial *mat = scene->mMaterials[i];
    prepareTextures(mat, i);
  }

  aiReleaseImport(scene);
}

void Model::processNode(const aiNode *node, const aiScene *scene,
                        glm::mat4 parentTrans) {
  NodeInfo info{
      .localTransform =
          parentTrans * GLMHelper::ConvertMatrix(node->mTransformation),
  };

  for (uint32_t i = 0; i < node->mNumMeshes; i++) {
    const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    info.meshes.emplace_back(processMesh(mesh, scene));
  }

  nodes.emplace_back(info);
  for (uint32_t i = 0; i < node->mNumChildren; i++)
    processNode(node->mChildren[i], scene, info.localTransform);
}

MeshInfo Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
  std::vector<MeshVertex> vertices;
  std::vector<uint16_t> indices;

  for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
    const aiVector3D p = mesh->mVertices[i];
    const aiVector3D n = mesh->mNormals[i];
    const aiVector3D ta = mesh->mTangents[i];
    const aiVector3D bi = mesh->mBitangents[i];
    const aiVector3D t =
        mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i] : aiVector3D(0.0f);

    vertices.emplace_back(MeshVertex{
        .pos = glm::vec3(p.x, p.y, p.z),
        .norm = glm::vec3(n.x, n.y, n.z),
        .uv = glm::vec2(t.x, t.y),
        .tan = glm::vec3(ta.x, ta.y, ta.z),
        .bitan = glm::vec3(bi.x, bi.y, bi.z),
    });
  }

  for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    for (uint32_t j = 0; j < 3; j++)
      indices.emplace_back(mesh->mFaces[i].mIndices[j]);

  MeshInfo meshInfo = {
      .verBuffer = ren->createBuffer({
          .usage = MAI::Storage_Buffer,
          .storage = MAI::StorageType_Device,
          .size = vertices.size() * sizeof(MeshVertex),
          .data = vertices.data(),
      }),
      .indexBuffer = ren->createBuffer({
          .usage = MAI::IndexBuffer,
          .storage = MAI::StorageType_Device,
          .size = indices.size() * sizeof(uint16_t),
          .data = indices.data(),
      }),
      .indicesSize = (uint32_t)indices.size(),
      .tex_id = mesh->mMaterialIndex,
  };

  return meshInfo;
}

MAI::Texture *Model::loadTexture(const char *filename) {
  for (std::string &str : loadTextures)
    if (str == filename)
      return nullptr;

  std::string path = dir + filename;

  MAI::Texture *tex = ImageLoader::loadFromFile(ren, path);

  loadTextures.emplace_back(filename);

  return tex;
}

void Model::prepareTextures(const aiMaterial *mat, uint32_t id) {
  MeshTextureInfo info;
  aiString str;
  aiColor4D color;

  if (mat->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS)
    info.baseColor = glm::vec4(color.r, color.g, color.b, color.a);

  if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &str) == AI_SUCCESS)
    info.diff = loadTexture(str.C_Str());
  else if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    info.diffColor = glm::vec4(color.r, color.g, color.b, color.a);

  if (mat->GetTexture(aiTextureType_NORMALS, 0, &str) == AI_SUCCESS)
    info.norm = loadTexture(str.C_Str());

  if (mat->GetTexture(aiTextureType_EMISSIVE, 0, &str) == AI_SUCCESS)
    info.emissive = loadTexture(str.C_Str());
  else if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
    info.emissiveColor = glm::vec4(color.r, color.g, color.b, color.a);

  if (mat->GetTexture(aiTextureType_GLTF_METALLIC_ROUGHNESS, 0, &str) ==
      AI_SUCCESS)
    info.arm = loadTexture(str.C_Str());

  info.id = id;

  textures.push_back(info);
}

Model::~Model() {
  for (auto &n : nodes)
    for (auto &m : n.meshes) {
      delete m.verBuffer;
      delete m.indexBuffer;
    }

  for (auto &tex : textures) {
    if (tex.diff)
      delete tex.diff;
    if (tex.norm)
      delete tex.norm;
    if (tex.emissive)
      delete tex.emissive;
    if (tex.arm)
      delete tex.arm;
  }
}
