#include "engine/inputs.h"
#include <algorithm>

static void setKeyCallback(GLFWwindow *window, int key, int scancode,
                           int action, int mods);
static void setMouseButtonCallback(GLFWwindow *window, int button, int action,
                                   int mods);

Inputs::Inputs(GLFWwindow *window) : window(window) {
  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, setKeyCallback);
  glfwSetMouseButtonCallback(window, setMouseButtonCallback);
}

static void setKeyCallback(GLFWwindow *window, int key, int scancode,
                           int action, int mods) {
  auto *input = static_cast<Inputs *>(glfwGetWindowUserPointer(window));
  input->keys[key] = action == GLFW_PRESS;

  if (mods & GLFW_MOD_SHIFT)
    input->mods[0] = true;
  else
    input->mods[0] = false;

  if (mods & GLFW_MOD_CONTROL)
    input->mods[1] = true;
  else
    input->mods[1] = false;

  if (mods & GLFW_MOD_ALT)
    input->mods[2] = true;
  else
    input->mods[2] = false;
}

static void setMouseButtonCallback(GLFWwindow *window, int button, int action,
                                   int mods) {
  auto *input = static_cast<Inputs *>(glfwGetWindowUserPointer(window));
  input->mouseKeys[button] = action == GLFW_PRESS;
}

void Inputs::getMousePos(double &xpos, double &ypos) {
  glfwGetCursorPos(window, &xpos, &ypos);
}

void Inputs::getWindowSize(int &width, int &height) {
  glfwGetFramebufferSize(window, &width, &height);
}
