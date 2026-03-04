#include "engine/scene/game_settings.h"
#include "imgui.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::string com = "game_settings";

void GameSettings::init() {
  // game state
  std::ifstream f(RESOURCES_PATH "states/game_state.json");
  json data = json::parse(f);
  name = data[com]["name"].get<std::string>();
  width = data[com]["width"].get<uint32_t>();
  height = data[com]["height"].get<uint32_t>();
  isFullScreen = data[com]["fullscreen"].get<bool>();
  msaa = data[com]["msaa"].get<uint32_t>();
  f.close();
};

void GameSettings::guiSettings() {
  // settings
  if (ImGui::TreeNode("Settings")) {
    ImGui::NewLine();
    if (ImGui::Button("Resolution"))
      ImGui::OpenPopup("res_settings");
    ImGui::SameLine();
    std::string res = std::to_string(width) + "x" + std::to_string(height);
    ImGui::Text(res.c_str(), nullptr);

    if (ImGui::BeginPopup("res_settings")) {
      if (ImGui::Selectable("1920x1080")) {
        width = 1920;
        height = 1080;
      }
      if (ImGui::Selectable("1300x760")) {
        width = 1300;
        height = 760;
      }
      if (ImGui::Selectable("1200x760")) {
        width = 1200;
        height = 800;
      }
      ImGui::EndPopup();
    }

    ImGui::NewLine();
    int fullScreen = (int)isFullScreen;
    if (ImGui::RadioButton("Fullscreen", &fullScreen, 1)) {
      // GLFWmonitor *monitor = glfwGetPrimaryMonitor();
      // const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      // glfwSetWindowMonitor(app->window, monitor, 0, 0, gameSettings.width,
      //                      gameSettings.height, mode->refreshRate);
      isFullScreen = (bool)fullScreen;
    }

    ImGui::SameLine();
    if (ImGui::RadioButton("Window", &fullScreen, 0)) {
      isFullScreen = (bool)fullScreen;
      // glfwSetWindowMonitor(app->window, nullptr, 0, 0, gameSettings.width,
      //                      gameSettings.height, 0);
    }

    ImGui::NewLine();
    ImGui::Text("Graphics");

    ImGui::NewLine();
    if (ImGui::Button("Anti Aliasing"))
      ImGui::OpenPopup("res_msaa");
    ImGui::SameLine();

    std::string name;
    if (msaa == 0)
      name = "off";
    else if (msaa == 1)
      name = "MSAA";
    // else if (msaa == 2)
    //   name = "TAA";

    ImGui::Text(name.c_str(), nullptr);

    if (ImGui::BeginPopup("res_msaa")) {
      if (ImGui::Selectable("off"))
        msaa = 0;
      if (ImGui::Selectable("MSAA"))
        msaa = 1;
      if (ImGui::Selectable("TAA"))
        msaa = 2;
      ImGui::EndPopup();
    }

    ImGui::TreePop();
  }
}

MAI::MSAASample GameSettings::getMSAASample() {
  if (msaa == 0)
    return MAI::Count_1_Bit;
  if (msaa == 1)
    return MAI::Count_4_Bit;

  assert(false);
}

void GameSettings::save() {
  // settings
  json settj;
  settj[com]["name"] = name;
  settj[com]["width"] = width;
  settj[com]["height"] = height;
  settj[com]["fullscreen"] = isFullScreen;
  settj[com]["graphics_mod"] = graphics_mode;
  settj[com]["msaa"] = msaa;

  std::ofstream file(RESOURCES_PATH "states/game_state.json");
  if (!file.is_open())
    assert(false);

  file << settj << std::endl;
}
