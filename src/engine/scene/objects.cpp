#include "engine/scene/objects.h"
#include "imgui.h"
#include <iostream>

Objects::Objects(MAI::Renderer *ren, VkFormat format, Textures *texs)
    : ren(ren), textures(texs) {
  cube = Shapes::getCube(ren);

  MAI::Shader *vert = ren->createShader(SHADERS_PATH "object.vert");
  MAI::Shader *frag = ren->createShader(SHADERS_PATH "object.frag");
  pipeline = ren->createPipeline({
      .vert = vert,
      .frag = frag,
      .depthFormat = format,
      .cullMode = MAI::CullMode::Back,
  });
  delete vert;
  delete frag;
}

void Objects::addObject(ObjectsInfo info) {
  uint32_t id;
  if (objects.empty())
    id = 0;
  else
    id = objects.size();

  objects.emplace_back(info);
  updateBuff = true;
}

void Objects::addObjects(std::vector<ObjectsInfo> &infos) {
  for (auto in : infos)
    objects.emplace_back(in);
  updateBuff = true;
}

void Objects::draw(MAI::CommandBuffer *buff, uint64_t buffPerFrame,
                   glm::mat4 pv) {
  if (objects.empty())
    return;

  if (updateBuff || !objectBuff)
    settingBuffers();

  struct PushConstant {
    glm::mat4 pv;
    uint64_t vertices;
    uint64_t objData;
    uint64_t bufData;
  } pc{
      .pv = pv,
      .vertices = ren->gpuAddress(cube.vertBuffer),
      .objData = ren->gpuAddress(objectBuff),
      .bufData = buffPerFrame,
  };

  buff->bindPipeline(pipeline);
  buff->cmdPushConstant(&pc);
  buff->bindIndexBuffer(cube.indexBuffer, 0, MAI::Uint16);
  buff->cmdDrawIndex(cube.indicesSize, objects.size());
}

void Objects::settingBuffers() {
  if (objectBuff)
    delete objectBuff;

  std::cout << "objects buff created" << std::endl;

  std::vector<ObjectPerFrame> objs;
  objs.reserve(objects.size());
  for (auto obj : objects) {
    glm::mat4 model = GLMHelper::toMat4(obj.pos, obj.rotate, obj.scale);
    ObjectPerFrame objPer{
        .model = model,
        .color = glm::vec4(obj.color, 1.0f),
        .tiling = obj.tiling,
        .ao = obj.ao,
        .metallic = obj.metallic,
        .roughness = obj.roughness,
        .albedoTex = 0,
        .normTex = 0,
        .armTex = 0,
    };
    Texture *tex = textures->getTextures(obj.tex_id);
    if (tex != nullptr)
      objPer.albedoTex = tex->albedo->getIndex();

    objs.emplace_back(objPer);
  }

  objectBuff = ren->createBuffer({
      .usage = MAI::Storage_Buffer,
      .storage = MAI::StorageType_Device,
      .size = objs.size() * sizeof(ObjectPerFrame),
      .data = objs.data(),
  });
  updateBuff = false;
}

void Objects::allObjectGui() {
  ImGui::NewLine();
  if (ImGui::TreeNode("Entities")) {
    for (uint32_t i = 0; i < objects.size(); i++) {
      std::string name = "cube " + std::to_string(i);
      if (ImGui::Selectable(name.c_str()))
        focuseId = i;
    }
    ImGui::TreePop();
  }
}

void Objects::objectGui() {
  if (focuseId == -1) {
    return;
  }

  if (const ImGuiViewport *v = ImGui::GetMainViewport()) {
    ImGui::SetNextWindowPos(
        {v->WorkPos.x + v->WorkSize.x - 15.0f, v->WorkPos.y + 100.0f},
        ImGuiCond_Always, {1.0f, 0.0f});

    ImGui::SetNextWindowSize({v->WorkSize.x * 0.3f, 0}, ImGuiCond_Always);
  }

  auto inputFloat3WithCommit = [&](const char *label, glm::vec3 &value) {
    ImGui::InputFloat3(label, glm::value_ptr(value));
    return ImGui::IsItemDeactivatedAfterEdit();
  };

  auto inputFloatWithCommit = [&](const char *label, float &value) {
    ImGui::InputFloat(label, &value);
    return ImGui::IsItemDeactivatedAfterEdit();
  };

  auto inputColorWithCommit = [&](const char *label, glm::vec3 &color) {
    ImGui::ColorEdit3(label, glm::value_ptr(color));
    return ImGui::IsItemDeactivatedAfterEdit();
  };

  ObjectsInfo &info = objects[focuseId];
  ImGui::Begin("Entity Properties");

  float val = info.tiling;
  if (inputFloatWithCommit("Tiling", val))
    if (val != info.tiling) {
      info.tiling = val;
      updateBuff = true;
    }

  val = info.ao;
  if (inputFloatWithCommit("AO", val))
    if (val != info.ao) {
      info.ao = val;
      updateBuff = true;
    }

  val = info.metallic;
  if (inputFloatWithCommit("Metallic", val))
    if (val != info.metallic) {
      info.metallic = val;
      updateBuff = true;
    }

  val = info.roughness;
  if (inputFloatWithCommit("Roughness", val))
    if (val != info.roughness) {
      info.roughness = val;
      updateBuff = true;
    }

  glm::vec3 values = info.color;
  if (ImGui::ColorEdit3("Color", glm::value_ptr(values))) {
    if (values != info.color) {
      info.color = values;
      updateBuff = true;
    }
  }

  values = info.pos;
  if (inputFloat3WithCommit("Pos", values))
    if (values != info.pos) {
      info.pos = values;
      updateBuff = true;
    }

  values = info.rotate;
  if (inputFloat3WithCommit("Rotate", values))
    if (values != info.rotate) {
      info.rotate = values;
      updateBuff = true;
    }

  values = info.scale;
  if (inputFloat3WithCommit("Scale", values))
    if (values != info.scale) {
      info.scale = values;
      updateBuff = true;
    }

  uint32_t id = info.tex_id;
  textures->guiWidget(id);

  if (id != info.tex_id) {
    info.tex_id = id;
    updateBuff = true;
  }

  ImGui::End();
}

Objects::~Objects() {
  delete objectBuff;
  delete pipeline;
  Shapes::deleteShape(cube);
}
