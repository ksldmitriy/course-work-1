#pragma once
#include "vk/instance.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

#include <glm/glm.hpp>
#include <imgui.h>

using namespace std;

enum class MouseButton { left = 1 << 0, right = 1 << 1, middle = 1 << 2 };

struct MouseButtonEvent {
  bool pressed;
  MouseButton button;
  bool shift_mod;
};

struct MouseMoveEvent {
  glm::fvec2 pos;
  glm::fvec2 delta;

  glm::ivec2 p_pos;
  glm::ivec2 p_delta;
};

struct MouseState {
  MouseButton pressed_buttons;

  glm::fvec2 pos;
  glm::ivec2 p_pos;

  bool shift_mod;
};

class Window {
private:
  static bool glfw_inited;
  static map<GLFWwindow *, Window *> windows_db;

  GLFWwindow *handle;
  VkSurfaceKHR surface;
  vk::Instance *instance;

  ImGuiIO *imgui_io;

  glm::ivec2 size;

  MouseState mouse_state;
  vector<MouseButtonEvent> mouse_button_events;
  vector<MouseMoveEvent> mouse_move_events;

  static void InitGLFW();

  static void StaticResizeCallback(GLFWwindow *window, int width, int height);
  static void StaticKeyCallback(GLFWwindow *window, int key, int scancode,
                                int action, int mods);
  static void StaticCursorPositionCallback(GLFWwindow *window, double x_pos,
                                           double y_pos);
  static void StaticMouseButtonCallback(GLFWwindow *window, int button,
                                        int action, int mods);

  glm::fvec2 PixelToRelative(glm::ivec2 p_pos);

  void ResizeCallback(int width, int height);
  void KeyCallback(int key, int scancode, int action, int mods);
  void CursorPositionCallback(double x_pos, double y_pos);
  void MouseButtonCallback(int button, int action, int mods);

  void CreateWindow();

public:
  Window();
  Window(Window &) = delete;
  Window &operator=(Window &) = delete;
  ~Window();

  void AttachInstance(vk::Instance &instance);

  void Destroy();

  void AttachImguiIO(ImGuiIO *io);

  GLFWwindow *GetHandle();
  void CreateSurface();
  void DestroySurface();
  VkSurfaceKHR GetSurface();

  MouseState GetMouseState();
  void GetEvents(vector<MouseButtonEvent> &key_events,
                 vector<MouseMoveEvent> &move_events);

  bool ShouldClose();
  static void PollEvents();

  static void GetInstanceExtensions(const char **&extensions,
                                    uint32_t &extensions_count);
};
