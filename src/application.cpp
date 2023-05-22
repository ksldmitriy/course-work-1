#include "application.hpp"

Application ::~Application() {
  staging_command_buffer->Dispose();

  command_pool->Dispose();

  instance_renderer.reset();

  vkDestroyRenderPass(device->GetHandle(), render_pass, nullptr);

  CleanupFramebuffers();

  CleanupSyncObjects();

  swapchain->Dispose();

  car_texture->Destroy();

  device->Dispose();

  INFO("application destroyed");
}

void Application::Run() {
  window = unique_ptr<Window>(new Window());

  InitVulkan();

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
    Update();

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
