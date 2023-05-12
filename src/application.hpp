#pragma once
#include "instance_renderer.hpp"
#include "logs.hpp"
#include "render_structs.hpp"
#include "vk/vulkan.hpp"
#include "window.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <memory>

chrono::high_resolution_clock::time_point typedef time_point;
chrono::high_resolution_clock::duration typedef duration;
const auto now = chrono::high_resolution_clock::now;

using namespace std;

class Application {
private:
  const int frames_in_flight = 2;

  time_point program_start;
  duration time_from_start;

  unique_ptr<InstanceRenderer> instance_renderer;

  unique_ptr<vk::Instance> instance;
  unique_ptr<vk::Device> device;

  VkSemaphore image_available_semaphore, render_finished_semaphore;
  VkFence fence;

  unique_ptr<Window> window;

  unique_ptr<vk::Swapchain> swapchain;

  VkRenderPass render_pass;

  vk::Queue graphics_queue;

  vector<VkFramebuffer> framebuffers;

  void InitVulkan();
  void Prepare();
  void RenderLoop();
  void Draw();
  void Render(uint32_t next_image_index);
  void Present(uint32_t next_image_index);
  void CreateInstanceRenderer();

  void ChangeSurface();

  void CleanupSyncObjects();
  void CleanupFramebuffers();
  
  void PreUpdate();
  void Update();
  void UpdateRenderData();

  void CreateRenderPass();
  void CreateFramebuffers();

  void CreateInstance();
  void CreateDevice();

  void CreateSyncObjects();

public:
  Application() = default;
  Application(Application &) = delete;
  Application &operator=(Application &) = delete;
  ~Application();

  void Run();
};
