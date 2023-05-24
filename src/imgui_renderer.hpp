#pragma once
#include "vk/vulkan.hpp"
#include "window.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imconfig.h>
#include <imgui.h>
#include <imgui_internal.h>

using namespace std;

struct ImguiRendererCreateInfo {
  vk::Instance *instance;
  vk::Device *device;
  vk::Queue queue;
  Window *window;

  uint32_t swapchain_min_image_count;

  vector<VkFramebuffer> framebuffers;

  VkExtent2D extent;
  VkRenderPass render_pass;
};

class ImguiRenderer {
private:
  vk::Instance *instance;
  vk::Device *device;
  vk::Queue queue;
  Window *window;

  static bool imgui_initied; 
  
  uint32_t swapchain_min_image_count;

  vector<VkFramebuffer> framebuffers;
  VkExtent2D extent;
  VkRenderPass render_pass;

  VkDescriptorSetLayout descriptor_set_layout;
  VkDescriptorPool descriptors_pool;
  VkDescriptorSet descriptor_set;
  VkPipelineLayout pipeline_layout;

  size_t sprites_capacity;
  size_t sprites_count;

  VkPipeline pipeline;

  VkDescriptorPool imgui_descriptor_pool;

  unique_ptr<vk::CommandPool> imgui_command_pool;
  unique_ptr<vk::CommandBuffer> render_command_buffer;
  unique_ptr<vk::CommandBuffer> one_time_command_buffer;

  void Init();
  void InitImgui();
  void CreateImguiDescriptorPool();
  void CreateCommandBuffer(uint32_t image_index);
  
public:
  ImguiRenderer(ImguiRendererCreateInfo &create_info);
  ImguiRenderer(ImguiRenderer &) = delete;
  ImguiRenderer operator=(ImguiRenderer &) = delete;
  ~ImguiRenderer();

  
  void Render(uint32_t image_index, VkSemaphore image_available_semaphore,
              VkSemaphore render_finished_semaphore, VkFence fence);
};
