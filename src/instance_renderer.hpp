#pragma once
#include "render_structs.hpp"
#include "vk/vulkan.hpp"
#include <memory>

struct InstanceRendererCreateInfo {
  vk::Device *device;
  vk::Queue queue;
  vector<VkFramebuffer> framebuffers;
  VkExtent2D extent;
  VkRenderPass render_pass;
};

class InstanceRenderer {
private:
  vk::Device *device;
  vk::Queue queue;

  unique_ptr<vk::DeviceMemory> vertex_buffer_memory, uniform_buffer_memory;
  unique_ptr<vk::Buffer> vertex_buffer, instance_buffer, uniform_buffer;

  vector<VkFramebuffer> framebuffers;
  VkExtent2D extent;
  VkRenderPass render_pass;

  VkDescriptorSetLayout descriptor_set_layout;
  VkDescriptorPool descriptors_pool;
  VkDescriptorSet descriptor_set;
  VkPipelineLayout pipeline_layout;

  VkPipeline pipeline;

  unique_ptr<vk::CommandPool> command_pool;
  vector<unique_ptr<vk::CommandBuffer>> command_buffers;

  void CreateUniformBuffer();
  void CreateDescriptorSetLayout();
  void CreateDescriptorPool();
  void AllocateDescriptorSet();
  void CreateDescriptors();
  void UpdateDescriptorSet();

  void CreateVertexInputBuffers();

  void CreatePipeline(VkExtent2D extent, VkRenderPass render_pass);
  void CreateCommandBuffer();

public:
  InstanceRenderer(InstanceRendererCreateInfo &create_info);
  ~InstanceRenderer();

  void Init();
  void Render(uint32_t image_index, VkSemaphore image_available_semaphore,
              VkSemaphore render_finished_semaphore, VkFence fence);
};
