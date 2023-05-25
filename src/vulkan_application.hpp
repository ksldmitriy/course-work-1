#pragma once
#include "logs.hpp"
#include "render_structs.hpp"
#include "window.hpp"
#include <chrono>
#include <memory>

#include <stb_image.h>

#include "camera.hpp"
#include "debug_renderer.hpp"
#include "imgui_renderer.hpp"
#include "instance_renderer.hpp"
#include "mesh_renderer.hpp"
#include "vk/vulkan.hpp"

chrono::high_resolution_clock::time_point typedef time_point;
chrono::high_resolution_clock::duration typedef duration;
const auto now = chrono::high_resolution_clock::now;

using namespace std;

class VulkanApplication {
private:
  unique_ptr<vk::Semaphore> image_available_semaphore,
      road_render_finished_semaphore, cars_render_finished_semaphore,
      debug_render_finished_semaphore, imgui_render_finished_semaphore;
  VkFence fence;

  unique_ptr<vk::CommandPool> command_pool;
  unique_ptr<vk::CommandBuffer> staging_command_buffer;

  vector<VkFramebuffer> framebuffers;

  unique_ptr<vk::Instance> instance;
  unique_ptr<vk::Device> device;

  unique_ptr<vk::Swapchain> swapchain;

  vk::Queue graphics_queue;

  unique_ptr<vk::Texture> car_texture;

  VkRenderPass road_render_pass, cars_render_pass, debug_render_pass,
      imgui_render_pass;

  void CreateRoadRenderPass();
  void CreateCarsRenderPass();
  void CreateDebugRenderPass();
  void CreateImguiRenderPass();

  void CreateRoadRenderer();
  void CreateInstanceRenderer();
  void CreateDebugRenderer();
  void CreateImguiRenderer();

  void CreateFramebuffers();
  void CreateSyncObjects();

  void Render(uint32_t next_image_index);
  void Present(uint32_t next_image_index);

  void CleanupSyncObjects();
  void CleanupFramebuffers();

  void CreateInstance(uint32_t glfw_extensions_count,
                      const char **glfw_extensions);
  void CreateDevice();

  void ChangeSurface();

  void CreateRenderers();
  void DestoyRenderers();

  void CreateTextures();

protected:
  unique_ptr<Window> window;

  unique_ptr<InstanceRenderer> instance_renderer;
  unique_ptr<DebugRenderer> debug_renderer;
  unique_ptr<ImguiRenderer> imgui_renderer;
  unique_ptr<MeshRenderer> road_renderer;

  void InitVulkan(uint32_t glfw_extensions_count, const char **glfw_extensions);
  void Prepare();

  void SetCamera(Camera camera);
  
  void Draw();

public:
  VulkanApplication();
  VulkanApplication(VulkanApplication &) = delete;
  VulkanApplication &operator=(VulkanApplication &) = delete;
  ~VulkanApplication();
};
