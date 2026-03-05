#pragma once

#include "engine/inputs.h"
#include "imgui.h"
#include <glm/ext.hpp>
#include <glm/glm.hpp>

enum CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 15.5f;
const float SENSITIVITY = 100.1f;
const float ZOOM = 45.0f;

struct Camera {
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;

  float Yaw;
  float Pitch;
  float MovementSpeed;
  float MouseSensitivit;
  float Zoom;

  Camera(glm::vec3 position = glm::vec3(0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
         float pitch = PITCH)
      : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
        MouseSensitivit(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraView();
  }

  glm::mat4 GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
  }

  void ProcessKeyboard(float deltaTime, Inputs &inputs) {
    float velocity = MovementSpeed * deltaTime;
    if (inputs.keys[GLFW_KEY_W])
      Position += Front * velocity;
    if (inputs.keys[GLFW_KEY_S])
      Position -= Front * velocity;
    if (inputs.keys[GLFW_KEY_A])
      Position -= Right * velocity;
    if (inputs.keys[GLFW_KEY_D])
      Position += Right * velocity;
  }

  bool firstMouse = true;
  float lastX = 0.0f;
  float lastY = 0.0f;
  void ProcessMouseMovement(Inputs &inputs, bool constraintPitch = true) {
    // if (ImGui::GetIO().WantCaptureMouse)
    //   return;

    if (inputs.mouseKeys[0]) {
      int width, height;
      inputs.getWindowSize(width, height);
      double mouseX, mouseY;
      inputs.getMousePos(mouseX, mouseY);
      mouseX = mouseX / width;
      mouseY = 1.0 - mouseY / height;

      float xpos = static_cast<float>(mouseX);
      float ypos = static_cast<float>(mouseY);

      if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
      }

      float xoffset = xpos - lastX;
      float yoffset = lastY - ypos;

      lastX = xpos;
      lastY = ypos;

      xoffset *= MouseSensitivit;
      yoffset *= MouseSensitivit;

      Yaw += xoffset;
      Pitch -= yoffset;

      if (constraintPitch) {
        if (Pitch > 80.0f)
          Pitch = 80.0f;
        if (Pitch < -80.0f)
          Pitch = -80.0f;
      }
    } else
      firstMouse = true;

    updateCameraView();
  }

  void ProcessMouseScroll(float yoffset) {
    Zoom -= yoffset;
    if (Zoom < 1.0f)
      Zoom = 1.0f;
    if (Zoom > 45.0f)
      Zoom = 45.0f;
  }

  void camerGui() {
    if (ImGui::TreeNode("Camera")) {
      ImGui::InputFloat("Camera Speed", &MovementSpeed);
      ImGui::InputFloat("Sensitivity", &MouseSensitivit);
      ImGui::TreePop();
    }
    ImGui::NewLine();
  }

private:
  void updateCameraView() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
  }
};
