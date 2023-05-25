#pragma once
#include "camera.hpp"
#include "render_structs.hpp"
#include "vk/vulkan.hpp"
#include <memory>

struct DebugRendererSettings {
  Camera camera;
};

struct DebugRendererCreateInfo {
  vk::Device *device;
  vk::Queue queue;
  vector<VkFramebuffer> framebuffers;

  VkExtent2D extent;
  VkRenderPass render_pass;

  DebugRendererSettings settings;
};

class DebugRenderer {
private:
  vk::Device *device;
  vk::Queue queue;

  unique_ptr<vk::DeviceMemory> vertex_buffer_memory, uniform_buffer_memory;
  unique_ptr<vk::Buffer> vertex_buffer, uniform_buffer;

  vector<VkFramebuffer> framebuffers;
  VkExtent2D extent;
  VkRenderPass render_pass;

  VkDescriptorSetLayout descriptor_set_layout;
  VkDescriptorPool descriptors_pool;
  VkDescriptorSet descriptor_set;
  VkPipelineLayout pipeline_layout;

  size_t lines_capacity;
  size_t lines_count;

  VkPipeline pipeline;

  DebugRendererSettings settings;

  unique_ptr<vk::CommandPool> command_pool;
  vector<unique_ptr<vk::CommandBuffer>> command_buffers;

  size_t GetOptimalSpritesCapacity(size_t sprites_count);

  void CreateUniformBuffer();
  void CreateDescriptorSetLayout();
  void CreateDescriptorPool();
  void AllocateDescriptorSet();
  void UpdateDescriptorSet();
  void CreateDescriptors();

  void CreateVertexBuffer();

  void CreatePipeline(VkExtent2D extent, VkRenderPass render_pass);
  void CreateCommandBuffers();

  void Init();

  void ApplyCameraSettings();
public:
  DebugRenderer(DebugRendererCreateInfo &create_info);
  DebugRenderer(DebugRenderer &) = delete;
  DebugRenderer operator=(DebugRenderer &) = delete;
  ~DebugRenderer();

  void Render(uint32_t image_index, VkSemaphore image_available_semaphore,
              VkSemaphore render_finished_semaphore, VkFence fence);

  void SetCamera(Camera camera);
  DebugRendererSettings GetSettings();

  void LoadLines(vector<Line> &lines);
};
