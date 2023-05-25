	#pragma once
#include "logs.hpp"
#include "render_structs.hpp"
#include "window.hpp"
#include <chrono>
#include <memory>
#include <deque>

#include <stb_image.h>

#include "camera.hpp"
#include "instance_renderer.hpp"
#include "vk/vulkan.hpp"

#include "vulkan_application.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

using namespace std;

class Application : protected VulkanApplication {
private:

  time_point program_start;
  time_point prev_frame;
  long double time_from_start;
  long double delta_time;
  vector<float> frame_time_history;
  vector<float> fps_history;
  float fps;
  
  VkRenderPass imgui_render_pass;
  
  void Prepare();
  void RenderLoop();

  void Update();
  void RenderUI();
  void DrawPerformanceMenu();
  
public:
  Application() = default;
  Application(Application &) = delete;
  Application &operator=(Application &) = delete;
  ~Application();

  void Run();
};
