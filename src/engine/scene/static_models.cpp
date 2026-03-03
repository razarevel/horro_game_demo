#include "engine/scene/static_models.h"
#include <imgui.h>
#include <iostream>

StaticModels::StaticModels(MAI::Renderer *ren, VkFormat format, Assets *assets,
                           MAI::MSAASample count)
    : ren(ren), assets(assets) {
  MAI::Shader *vert = ren->createShader(SHADERS_PATH "model.vert");
  MAI::Shader *frag = ren->createShader(SHADERS_PATH "model.frag");
  pipeline = ren->createPipeline({
      .vert = vert,
      .frag = frag,
      .depthFormat = format,
      .sampleCount = count,
      .cullMode = MAI::CullMode::Back,
  });
  delete vert;
  delete frag;
}

void StaticModels::addmodel(StaticModelInfo info) {
  uint32_t id = models.empty() ? 1 : models.size() + 1;
  info.id = id;
  models.emplace_back(info);
  update = true;
}

void StaticModels::draw(MAI::CommandBuffer *buff, glm::mat4 pv,
                        uint64_t buffAddress, glm::vec3 cameraPos) {
  if (models.empty())
    return;

  buff->bindPipeline(pipeline);
  struct PushConstant {
    glm::mat4 pv;
    glm::mat4 model;
    glm::vec4 baseColor;
    uint32_t albedoTex;
    uint32_t normTex;
    uint32_t armTex;
    uint64_t vtx;
    uint64_t buffData;
  } pc;
  for (auto md : models) {

    glm::mat4 model = GLMHelper::toMat4(md.pos, md.rotate, md.scale);
    pc = {
        .pv = pv,
        .model = glm::mat4(1.0f),
        .buffData = buffAddress,
    };

    auto &nodes = md.model->model->getNodes();
    auto &textures = md.model->model->getTextures();
    for (auto &n : nodes) {
      pc.model = glm::mat4(1.0f);
      pc.model = model * n.localTransform;
      for (auto &mesh : n.meshes) {
        auto &tex = textures[mesh.tex_id];
        if (tex.diff)
          pc.albedoTex = tex.diff->getIndex();
        if (tex.norm)
          pc.normTex = tex.norm->getIndex();
        if (tex.arm)
          pc.armTex = tex.arm->getIndex();

        pc.baseColor = tex.baseColor;

        pc.vtx = ren->gpuAddress(mesh.verBuffer);

        buff->cmdPushConstant(&pc);
        buff->bindIndexBuffer(mesh.indexBuffer, 0, MAI::Uint16);
        buff->cmdDrawIndex(mesh.indicesSize);
      }
    }
  }
}

void StaticModels::allModelGui() {
  ImGui::NewLine();
  if (ImGui::TreeNode("Static Models")) {
    for (auto md : models) {
      std::string name = md.model->name.c_str() + std::to_string(md.id);
      if (ImGui::Selectable(name.c_str()))
        focusId = --md.id;
    }
    ImGui::TreePop();
  }
}

void StaticModels::modelGui() {
  if (focusId == -1)
    return;

  StaticModelInfo &md = models[focusId];

  auto inputFloat3WithCommit = [&](const char *label, glm::vec3 &value) {
    ImGui::InputFloat3(label, glm::value_ptr(value));
    return ImGui::IsItemDeactivatedAfterEdit();
  };

  auto inputFloatWithCommit = [&](const char *label, float &value) {
    ImGui::InputFloat(label, &value);
    return ImGui::IsItemDeactivatedAfterEdit();
  };

  ImGui::Begin(md.model->name.c_str());

  if (ImGui::Button("Delete")) {
    models.erase(models.begin() + focusId);
    focusId--;
  }

  glm::vec3 val = md.pos;
  if (inputFloat3WithCommit("Pos", val))
    if (val != md.pos)
      md.pos = val;

  val = md.rotate;
  if (inputFloat3WithCommit("Rotate", val))
    if (val != md.rotate)
      md.rotate = val;

  float value = md.scale;
  if (inputFloatWithCommit("Scale", value))
    if (value != md.scale)
      md.scale = value;

  ImGui::End();
}

StaticModels::~StaticModels() { delete pipeline; }
