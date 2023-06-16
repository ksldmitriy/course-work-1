#pragma once
#include "render_structs.hpp"
#include "vk/vulkan.hpp"

using namespace std;

struct TextureRendererCreateInfo {
  vk::Device *device;
  vk::Queue queue;
  vector<VkFramebuffer> framebuffers;

  VkExtent2D extent;
  VkRenderPass render_pass;

  vk::ImageView *texture_view;
};

class TextureRenderer {
private:
  struct Vertex {
    glm::fvec2 pos, tex;
  };

  struct UniformData {
    int dump;
  };

public:
  struct RenderInfo {
    VkSemaphore wait_semaphore, signal_semaphore;
    VkFramebuffer framebuffer;
    VkRenderPass render_pass;
  };

private:
  vk::Device *device;
  vk::Queue queue;

  vector<VkFramebuffer> framebuffers;

  vk::ImageView *texture_view;
  VkSampler texture_sampler;

  VkExtent2D extent;

  VkPipeline pipeline;

  unique_ptr<vk::DeviceMemory> vertex_buffer_memory, uniform_buffer_memory;
  unique_ptr<vk::Buffer> vertex_buffer, uniform_buffer;

  unique_ptr<vk::CommandPool> command_pool;
  unique_ptr<vk::CommandBuffer> command_buffer;

  VkDescriptorSetLayout descriptor_set_layout;
  VkDescriptorPool descriptors_pool;
  VkDescriptorSet descriptor_set;
  VkPipelineLayout pipeline_layout;

  void CreateVertexBuffer();
  void CreateUniformBuffer();
  void CreateTextureSampler();
  void CreatePipeline(VkRenderPass render_pass);

  void CreateDescriptorSetLayout();
  void CreateDescriptorPool();
  void AllocateDescriptorSet();

  void UpdateDescriptorSet();

  void CreateCommandBuffers();
  void WriteCommandBuffer(RenderInfo &render_info);

  void Init(VkRenderPass render_pass);

public:
  TextureRenderer(vk::Device *device, TextureRendererCreateInfo &create_info);

  VkCommandBuffer Render(RenderInfo &render_info);
};
