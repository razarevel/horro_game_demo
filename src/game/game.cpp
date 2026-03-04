#include "game/game.h"

Game::Game() {
  gameSettings.init();
  load();
}

void Game::run() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    if (!menu->ready) {
      if (!blackScreen())
        continue;
    } else {
      menu->mainMenu();
    }
  }
  ren->waitDeviceIdle();
}

bool Game::blackScreen() {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  if (!width || !height)
    return false;

  MAI::CommandBuffer *buff = ren->acquireCommandBuffer();
  buff->cmdBeginRendering({
      .clearColor = {0, 0, 0, 0},
  });
  buff->cmdEndRendering();
  ren->submit();
  delete buff;
  return true;
}

void Game::load() {
  MAI::WindowInfo info{
      .width = gameSettings.width,
      .height = gameSettings.height,
      .appName = gameSettings.name.c_str(),
      .isFullscreen = gameSettings.isFullScreen,
      .allowResize = true,
  };

  window = MAI::initWindow(info);
  ren = MAI::initVulkanWithSwapChain(window, info.appName);
  inputs = new Inputs(window);
  menu = new Menu(ren, gameSettings, inputs);
}

void Game::save() { gameSettings.save(); }

Game::~Game() {
  delete menu;
  delete inputs;
  glfwDestroyWindow(window);
  glfwTerminate();
  delete ren;
}
