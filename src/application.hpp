#pragma once
#include "logs.hpp"
#include "render_structs.hpp"
#include "window.hpp"
#include <chrono>
#include <memory>

#include <stb_image.h>

#include "instance_renderer.hpp"
#include "camera.hpp"
#include "vk/vulkan.hpp"

#include "vulkan_application.hpp"

using namespace std;

class Application :protected VulkanApplication{
private:

  void Prepare();
  void RenderLoop();
public:
  Application() = default;
  Application(Application &) = delete;
  Application &operator=(Application &) = delete;
  ~Application();

  void Run();
};
