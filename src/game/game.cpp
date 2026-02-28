#include "game/game.h"
#include "engine/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

struct BufferPerFrame {
  glm::vec4 lightPos;
  glm::vec4 lightColor;
  glm::vec4 cameraPos;
};

struct GameSettings {
  std::string name = "Game";
  uint32_t width = 1200;
  uint32_t height = 800;
  bool isFullScreen = false;
  uint32_t graphics_mode = 0;
  uint32_t msaa = 1;
} gameSettings;

Game::Game() {
  initGame();
  load();
  // prepare the resources
  buffPerFrame = ren->createBuffer({
      .usage = MAI::Storage_Buffer,
      .storage = MAI::StorageType_Device,
      .size = sizeof(BufferPerFrame),
  });
}

void Game::run() {

  glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 lightColor = glm::vec3(255.0f, 255.0f, 255.0f);
  app->beforeDraw = [&](MAI::CommandBuffer *buff, uint32_t width,
                        uint32_t height, float ratio, float deltaSecond) {
    BufferPerFrame buffPer{
        .lightPos = glm::vec4(lightPos, 1.0f),
        .lightColor = glm::vec4(lightColor, 1.0f),
        .cameraPos = glm::vec4(app->camera->Position, 1.0f),
    };
    buff->update(buffPerFrame, &buffPer, sizeof(buffPer));
  };

  app->run([&](MAI::CommandBuffer *buff, uint32_t width, uint32_t height,
               float ratio, float deltaSecond) {
    glm::mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);
    p[1][1] *= -1;
    glm::mat4 view = app->camera->GetViewMatrix();
    buff->cmdBindDepthState({.depthWriteEnable = true, .compareOp = MAI::Less});
    objects->draw(buff, ren->gpuAddress(buffPerFrame), p * view);
    buff->cmdBindDepthState({});
    guiWidget();
  });
}

void Game::guiWidget() {
  if (const ImGuiViewport *v = ImGui::GetMainViewport()) {
    ImGui::SetNextWindowPos({v->WorkPos.x, v->WorkPos.y}, ImGuiCond_Always,
                            {0.0f, 0.0f});
    ImGui::SetNextWindowSize({v->WorkSize.x * 0.3f, v->WorkSize.y},
                             ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.50f);
  }
  ImGui::Begin("Viewport");
  if (ImGui::Button("Save"))
    save();
  // settings
  if (ImGui::TreeNode("Settings")) {
    ImGui::NewLine();
    if (ImGui::Button("Resolution"))
      ImGui::OpenPopup("res_settings");
    ImGui::SameLine();
    std::string res = std::to_string(gameSettings.width) + "x" +
                      std::to_string(gameSettings.height);
    ImGui::Text(res.c_str(), nullptr);

    if (ImGui::BeginPopup("res_settings")) {
      if (ImGui::Selectable("1920x1080")) {
        gameSettings.width = 1920;
        gameSettings.height = 1080;
      }
      if (ImGui::Selectable("1300x760")) {
        gameSettings.width = 1300;
        gameSettings.height = 760;
      }
      if (ImGui::Selectable("1200x760")) {
        gameSettings.width = 1200;
        gameSettings.height = 800;
      }
      ImGui::EndPopup();
    }

    ImGui::NewLine();
    int fullScreen = (int)gameSettings.isFullScreen;
    if (ImGui::RadioButton("Fullscreen", &fullScreen, 1))
      gameSettings.isFullScreen = (bool)fullScreen;

    ImGui::SameLine();
    if (ImGui::RadioButton("Window", &fullScreen, 0))
      gameSettings.isFullScreen = (bool)fullScreen;

    ImGui::NewLine();
    ImGui::Text("Graphics");
    int selected = gameSettings.graphics_mode;
    if (ImGui::RadioButton("LOW", &selected, 0))
      gameSettings.graphics_mode = selected;
    ImGui::SameLine();
    if (ImGui::RadioButton("Medium", &selected, 1))
      gameSettings.graphics_mode = selected;
    ImGui::SameLine();
    if (ImGui::RadioButton("High", &selected, 2))
      gameSettings.graphics_mode = selected;

    ImGui::NewLine();
    if (ImGui::Button("MSAA"))
      ImGui::OpenPopup("res_msaa");
    ImGui::SameLine();

    std::string name = "MSAA x" + std::to_string(gameSettings.msaa);
    ImGui::Text(name.c_str(), nullptr);

    if (ImGui::BeginPopup("res_msaa")) {
      for (uint32_t i = 1; i < 6; i++) {
        std::string mssa = "MSAA x" + std::to_string(i);
        if (ImGui::Selectable(mssa.c_str())) {
          gameSettings.msaa = i;
        }
      }
      ImGui::EndPopup();
    }

    ImGui::TreePop();
  }

  ImGui::NewLine();
  ImGui::Text("Add Objects");
  if (ImGui::Button("Cube")) {
    glm::vec3 &camPos = app->camera->Position;
    objects->addObject({
        .type = SHAPE,
        .object_id = 0,
        .tiling = 1.0f,
        .color = glm::vec3(1.0f),
        .pos = glm::vec3(camPos.x, camPos.y, camPos.z - 3.0),
        .rotate = glm::vec3(0.0f),
        .scale = glm::vec3(1.0f),
    });
  }

  objects->allObjectGui();
  ImGui::End();

  objects->objectGui();
}

std::vector<ObjectsInfo> infos;
void Game::initGame() {
  // game state
  std::ifstream f(RESOURCES_PATH "states/game_state.json");
  json data = json::parse(f);
  gameSettings.name = data["game_settings"]["name"].get<std::string>();
  gameSettings.width = data["game_settings"]["width"].get<uint32_t>();
  gameSettings.height = data["game_settings"]["height"].get<uint32_t>();
  gameSettings.isFullScreen = data["game_settings"]["fullscreen"].get<bool>();
  gameSettings.msaa = data["game_settings"]["msaa"].get<uint32_t>();
  f.close();

  // static objects
  f = std::ifstream(RESOURCES_PATH "states/static_objects.json");
  data = json::parse(f);

  for (json::iterator it = data.begin(); it != data.end(); it++) {
    ObjectsInfo info;
    info.id = (*it)["id"].get<uint32_t>();
    info.object_id = (*it)["object_id"].get<uint32_t>();
    info.tex_id = (*it)["tex_id"].get<uint32_t>();
    info.tiling = (*it)["tiling"].get<float>();
    info.ao = (*it)["ao"].get<float>();
    info.metallic = (*it)["metallic"].get<float>();
    info.roughness = (*it)["roughness"].get<float>();

    std::string type = (*it)["type"].get<std::string>();
    if (type == "shape")
      info.type = SHAPE;

    std::vector<float> f3 = (*it)["pos"].get<std::vector<float>>();
    std::cout << "parsing 4" << std::endl;
    info.pos = glm::vec3(f3[0], f3[1], f3[2]);
    f3 = (*it)["rotate"].get<std::vector<float>>();
    info.rotate = glm::vec3(f3[0], f3[1], f3[2]);
    f3 = (*it)["scale"].get<std::vector<float>>();
    info.scale = glm::vec3(f3[0], f3[1], f3[2]);
    infos.emplace_back(info);
  }
}

void Game::load() {
  MAI::WindowInfo info{
      .width = gameSettings.width,
      .height = gameSettings.height,
      .appName = gameSettings.name.c_str(),
      .isFullscreen = gameSettings.isFullScreen,
  };
  app = new MaiApp(info);
  ren = app->ren;
  textures = new Textures(ren);

  objects = new Objects(ren, app->depthTexture->getDeptFormat(), textures);
  objects->addObjects(infos);
}

void Game::save() {
  // settings
  json settj;
  std::string com = "game_settings";
  settj[com]["name"] = gameSettings.name;
  settj[com]["width"] = gameSettings.width;
  settj[com]["height"] = gameSettings.height;
  settj[com]["fullscreen"] = gameSettings.isFullScreen;
  settj[com]["graphics_mod"] = gameSettings.graphics_mode;
  settj[com]["msaa"] = gameSettings.msaa;

  std::ofstream file(RESOURCES_PATH "states/game_state.json");
  if (!file.is_open())
    assert(false);

  file << settj << std::endl;

  // saving objects
  infos.clear();
  infos = objects->getObjectsInfos();
  if (!infos.empty()) {
    json js = json::array();
    for (auto &data : infos) {
      json j;
      j["id"] = data.id;
      j["type"] = data.type == SHAPE ? "shape" : "asset";
      j["object_id"] = data.object_id;
      j["tex_id"] = data.tex_id;
      j["tiling"] = data.tiling;
      j["ao"] = data.ao;
      j["metallic"] = data.metallic;
      j["roughness"] = data.roughness;

      std::vector<float> value{data.color[0], data.color[1], data.color[2]};
      j["color"] = value;
      value = {data.pos[0], data.pos[1], data.pos[2]};
      j["pos"] = value;
      value = {data.rotate[0], data.rotate[1], data.rotate[2]};
      j["rotate"] = value;
      value = {data.scale[0], data.scale[1], data.scale[2]};
      j["scale"] = value;
      js.push_back(j);
    }

    file = std::ofstream(RESOURCES_PATH "states/static_objects.json");
    if (!file.is_open())
      assert(false);

    file << js << std::endl;
  }

  std::cout << "saved" << std::endl;
}

Game::~Game() {
  delete buffPerFrame;
  delete objects;
  delete textures;
  delete app;
}
