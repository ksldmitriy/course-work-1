	#pragma once
#include "logs.hpp"
#include "render_structs.hpp"
#include "window.hpp"
#include <chrono>
#include <memory>

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
  duration time_from_start;

  VkRenderPass imgui_render_pass;
  
  void Prepare();
  void RenderLoop();

  void ChangeSurface();

public:
  Application() = default;
  Application(Application &) = delete;
  Application &operator=(Application &) = delete;
  ~Application();

  void Run();
};
