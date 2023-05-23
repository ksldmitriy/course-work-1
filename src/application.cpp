#include "application.hpp"

Application ::~Application() {
  PreDestructor();
  INFO("application destroyed");
}

void Application::Run() {
  window = unique_ptr<Window>(new Window());

  uint32_t glfw_extensions_count;
  const char **glfw_extensions;

  window->GetInstanceExtensions(glfw_extensions, glfw_extensions_count);

  InitVulkan(glfw_extensions_count, glfw_extensions);

  window->AttachInstance(*instance);

  window->CreateSurface();

  swapchain = make_unique<vk::Swapchain>(*device, window->GetSurface());

  Prepare();

  RenderLoop();
}

void Application::Prepare() { VulkanApplication::Prepare(); }

void Application::RenderLoop() {
  DEBUG("render loop launched");

  while (!window->ShouldClose()) {

    try {
      Draw();
    } catch (vk::AcquireNextImageFailedException e) {
      ChangeSurface();
    } catch (vk::PresentFailedException e) {
      ChangeSurface();
    }

    Window::PollEvents();
  }

  DEBUG("render loop exit");
}

void Application::ChangeSurface() {

  vkDeviceWaitIdle(device->GetHandle());

  swapchain.reset();

  window->DestroySurface();
  window->CreateSurface();

  VulkanApplication::ChangeSurface(window->GetSurface());
}
