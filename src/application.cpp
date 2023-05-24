#include "application.hpp"

Application ::~Application() { INFO("application destroyed"); }

void Application::Run() {
  Prepare();

  RenderLoop();
}

void Application::Prepare() { VulkanApplication::Prepare(); }

void Application::RenderLoop() {
  DEBUG("render loop launched");

  while (!window->ShouldClose()) {

    try {

      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      ImGui::ShowDemoWindow();

      ImGui::Render();

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
