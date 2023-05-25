#pragma once
#include "camera.hpp"
#include "render_structs.hpp"
#include "vk/vulkan.hpp"
#include <memory>

struct MeshRendererSettings {
  Camera camera;
};

struct MeshRendererCreateInfo {
  vk::Device *device;
  vk::Queue queue;
  vector<VkFramebuffer> framebuffers;

  VkExtent2D extent;
  VkRenderPass render_pass;

  MeshRendererSettings settings;
};

class MeshRenderer {
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

  size_t mesh_capacity;
  size_t mesh_count;

  VkPipeline pipeline;

  MeshRendererSettings settings;

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
  MeshRenderer(MeshRendererCreateInfo &create_info);
  MeshRenderer(MeshRenderer &) = delete;
  MeshRenderer operator=(MeshRenderer &) = delete;
  ~MeshRenderer();

  void Render(uint32_t image_index, VkSemaphore image_available_semaphore,
              VkSemaphore render_finished_semaphore, VkFence fence);

  void SetCamera(Camera camera);
  MeshRendererSettings GetSettings();

  void LoadMehs(vector<Triangle> &mesh);
};
