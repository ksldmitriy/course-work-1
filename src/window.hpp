#pragma once
#include "vk/instance.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

using namespace std;

namespace vk {
class Instance;
}

class Window {
private:
  static bool glfw_inited;
  static map<GLFWwindow *, Window *> windows_db;

  GLFWwindow *handle;
  VkSurfaceKHR surface;
  vk::Instance *instance;

  static void InitGLFW();

  static void StaticKeyCallback(GLFWwindow *window, int key, int scancode,
                                int action, int mods);
  static void StaticResizeCallback(GLFWwindow *window, int width, int height);

  void CreateWindow();

  void KeyCallback(int key, int scancode, int action, int mods);
  void ResizeCallback(int width, int height);

public:
  Window();
  Window(Window &) = delete;
  Window &operator=(Window &) = delete;
  ~Window();

  void AttachInstance(vk::Instance &instance);

  void Destroy();

  void CreateSurface();
  void DestroySurface();
  VkSurfaceKHR GetSurface();

  bool ShouldClose();
  static void PollEvents();

  static void GetInstanceExtensions(const char **&extensions,
                                    uint32_t &extensions_count);
};
