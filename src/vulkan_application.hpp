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

chrono::high_resolution_clock::time_point typedef time_point;
chrono::high_resolution_clock::duration typedef duration;
const auto now = chrono::high_resolution_clock::now;

using namespace std;

class VulkanApplication {
private:
  void CreateRenderPass();
  void CreateFramebuffers();
  void CreateSyncObjects();

  VkSemaphore image_available_semaphore, render_finished_semaphore;
  VkFence fence;

  unique_ptr<vk::CommandPool> command_pool;
  unique_ptr<vk::CommandBuffer> staging_command_buffer;

  void Render(uint32_t next_image_index);
  void Present(uint32_t next_image_index);

  void CleanupSyncObjects();
  void CleanupFramebuffers();

  vector<VkFramebuffer> framebuffers;

  void CreateInstanceRenderer();

  void CreateInstance(uint32_t glfw_extensions_count,
                      const char **glfw_extensions);
  void CreateDevice();

protected:
  unique_ptr<vk::Instance> instance;
  unique_ptr<vk::Device> device;

  unique_ptr<InstanceRenderer> instance_renderer;

  unique_ptr<vk::Swapchain> swapchain;

  VkRenderPass render_pass;

  vk::Queue graphics_queue;

  unique_ptr<vk::Texture> car_texture;

  void InitVulkan(uint32_t glfw_extensions_count, const char **glfw_extensions);
  void Prepare();

  void Draw();

  void ChangeSurface(VkSurfaceKHR surface);

  void CreateTextures();

  void PreDestructor();

public:
  VulkanApplication();
  VulkanApplication(VulkanApplication &) = delete;
  VulkanApplication &operator=(VulkanApplication &) = delete;
  ~VulkanApplication();
};
