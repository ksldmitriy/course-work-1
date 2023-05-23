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

using namespace std;

class Application : protected VulkanApplication {
private:
  unique_ptr<Window> window;

  time_point program_start;
  duration time_from_start;

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
