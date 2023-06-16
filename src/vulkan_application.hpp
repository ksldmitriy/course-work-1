#pragma once
#include "logs.hpp"
#include "window.hpp"
#include <chrono>
#include <memory>

#include <stb_image.h>

#include "vk/vulkan.hpp"
#include "texture_renderer.hpp"

using namespace std;

class VulkanApplication {
private:
  VkFence fence;

  unique_ptr<vk::Instance> instance;
  unique_ptr<vk::Device> device;

  vector<VkFramebuffer> framebuffers;

  unique_ptr<vk::Swapchain> swapchain;

  vk::Queue graphics_queue;

  unique_ptr<vk::Texture> car_texture;

  unique_ptr<vk::Image> pheromone_map_image;
  unique_ptr<vk::ImageView> pheromone_map_view;

  unique_ptr<TextureRenderer> texture_renderer;
  
  VkRenderPass pheromone_render_pass;
  
  bool surface_changed = false;

  static constexpr glm::ivec2 map_size = {100, 100};
  
  void CreateFramebuffers();
  void CreateSyncObjects();

  void Render(uint32_t next_image_index);
  void Present(uint32_t next_image_index);

  void CleanupSyncObjects();
  void CleanupFramebuffers();

  void CreateInstance(uint32_t glfw_extensions_count,
                      const char **glfw_extensions);
  void CreateDevice();

  void CreatePheromoneMap();
  
  void CreateTextureRenderPass();

  void CreateTextureRenderer();
  
  void ChangeSurface();

protected:
  unique_ptr<Window> window;

  void InitVulkan(uint32_t glfw_extensions_count, const char **glfw_extensions);
  void Prepare();

  bool IsSurfaceChanged();

  void Draw();

public:
  VulkanApplication();
  VulkanApplication(VulkanApplication &) = delete;
  VulkanApplication &operator=(VulkanApplication &) = delete;
  ~VulkanApplication();
};
