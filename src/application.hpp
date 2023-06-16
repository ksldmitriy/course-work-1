#pragma once
#include "window.hpp"
#include <chrono>

#include "vulkan_application.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imconfig.h>
#include <imgui.h>
#include <imgui_internal.h>

using namespace std;
chrono::high_resolution_clock::time_point typedef time_point;
chrono::high_resolution_clock::duration typedef duration;
const auto now = chrono::high_resolution_clock::now;

class Application : protected VulkanApplication {
private:
  struct TimeInfo {
    time_point program_start;
    time_point prev_frame;
    long double time_from_start;
    long double delta_time;
    vector<float> frame_time_history;
    vector<float> fps_history;
    float fps;
  };

  TimeInfo time_info;

  void Prepare();

  void ChangeSufaceCallback();

  void MainLoop();

  void Update();
  void UpdateTime();

  void ProcessEvents();
  void ProcessMouseMoveEvent(MouseMoveEvent event);
  void ProcessMouseButtonEvent(MouseButtonEvent event);

  void RenderUI();

public:
  Application() = default;
  Application(Application &) = delete;
  Application &operator=(Application &) = delete;
  ~Application();

  void Run();
};
