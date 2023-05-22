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

  glm::fvec2 sprite_size;
  vk::Image* texture;
};

class InstanceRenderer {
private:
  vk::Device *device;
  vk::Queue queue;

  unique_ptr<vk::DeviceMemory> vertex_buffer_memory, instance_buffer_memory, uniform_buffer_memory;
  unique_ptr<vk::Buffer> vertex_buffer, instance_buffer, uniform_buffer;

  vk::Image* texture;
  unique_ptr<vk::ImageView> texture_view;
  VkSampler texture_sampler;
  
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

  glm::fvec2 sprite_size;
  
  unique_ptr<vk::CommandPool> command_pool;
  vector<unique_ptr<vk::CommandBuffer>> command_buffers;

  size_t GetOptimalSpritesCapacity(size_t sprites_count);
  
  void CreateUniformBuffer();
  void CreateDescriptorSetLayout();
  void CreateDescriptorPool();
  void AllocateDescriptorSet();
  void CreateDescriptors();
  void UpdateDescriptorSet();

  void CreateTextureSampler();
  
  void CreateVertexBuffer();
  void CreateInstanceBuffers(size_t size);
  
  void CreatePipeline(VkExtent2D extent, VkRenderPass render_pass);
  void CreateCommandBuffer();

  void Init();
public:
  InstanceRenderer(InstanceRendererCreateInfo &create_info);
  ~InstanceRenderer();

  void Render(uint32_t image_index, VkSemaphore image_available_semaphore,
              VkSemaphore render_finished_semaphore, VkFence fence);

  void SetCamera(glm::fvec2 pos, float scale);
  
  void LoadSprites(vector<Transforn2D>& sprites);
};
