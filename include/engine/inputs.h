#pragma once
#include <GLFW/glfw3.h>

struct Inputs {
  Inputs(GLFWwindow *window);

  bool keys[128] = {false};
  bool mouseKeys[3] = {false};
  bool mods[3] = {false};

  void getMousePos(double &xpos, double &ypos);
  void getWindowSize(int &width, int &height);

private:
  GLFWwindow *window;
};
