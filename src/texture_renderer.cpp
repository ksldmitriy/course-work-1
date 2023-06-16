#include "texture_renderer.hpp"

TextureRenderer::TextureRenderer(vk::Device *device,
                                 TextureRendererCreateInfo &create_info) {
  device = create_info.device;
  queue = create_info.queue;
  framebuffers = create_info.framebuffers;
  texture_view = create_info.texture_view;
  create_info.extent = extent;

  Init(create_info.render_pass);
}

void TextureRenderer::Init(VkRenderPass render_pass) {
  CreateVertexBuffer();
  CreateUniformBuffer();

  CreateTextureSampler();

  CreateDescriptorSetLayout();
  CreateDescriptorPool();
  AllocateDescriptorSet();

  UpdateDescriptorSet();

  CreatePipeline(render_pass);
}

VkCommandBuffer TextureRenderer::Render(RenderInfo &render_info) {
  WriteCommandBuffer(render_info);

  return command_buffer->GetHandle();
}

void TextureRenderer::CreatePipeline(VkRenderPass render_pass) {
  unique_ptr<vk::ShaderModule> vertex_shader =
      make_unique<vk::ShaderModule>(*device, "shaders/texture_vert.spv");
  unique_ptr<vk::ShaderModule> fragment_shader =
      make_unique<vk::ShaderModule>(*device, "shaders/texture_frag.spv");

  VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info =
      vk::pipeline_shader_stage_create_info_template;
  vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertex_shader_stage_create_info.module = vertex_shader->GetHandle();
  vertex_shader_stage_create_info.pName = "main";

  VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info =
      vk::pipeline_shader_stage_create_info_template;
  fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragment_shader_stage_create_info.module = fragment_shader->GetHandle();
  fragment_shader_stage_create_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[2] = {
      vertex_shader_stage_create_info, fragment_shader_stage_create_info};

  vector<VkVertexInputBindingDescription> binding_description;

  VkVertexInputBindingDescription vertex_input_description;
  vertex_input_description.binding = 0;
  vertex_input_description.stride = sizeof(Vertex);
  vertex_input_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  VkVertexInputAttributeDescription attribute_descriptions[2];

  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(Vertex, pos);

  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].location = 1;
  attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(Vertex, tex);

  VkPipelineVertexInputStateCreateInfo vertex_input =
      vk::vertex_input_create_info_template;
  vertex_input.pVertexBindingDescriptions = &vertex_input_description;
  vertex_input.vertexBindingDescriptionCount = 1;
  vertex_input.pVertexAttributeDescriptions = attribute_descriptions;
  vertex_input.vertexAttributeDescriptionCount = 2;

  VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info =
      vk::pipeline_input_assembly_create_info_template;
  input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkViewport viewport;
  viewport.x = 0;
  viewport.y = 0;
  viewport.width = extent.width;
  viewport.height = extent.height;
  viewport.minDepth = 0;
  viewport.maxDepth = 1;

  VkRect2D scissor;
  scissor.offset = {0, 0};
  scissor.extent = extent;

  VkPipelineViewportStateCreateInfo viewport_state_create_info =
      vk::pipeline_viewport_state_create_info_template;
  viewport_state_create_info.pViewports = &viewport;
  viewport_state_create_info.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterization_state_create_info =
      vk::pipeline_rasterization_state_create_info_template;
  rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;

  VkPipelineMultisampleStateCreateInfo multisample_create_info =
      vk::pipeline_multisample_state_create_info_template;

  VkPipelineColorBlendAttachmentState color_blend_attachment;
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo color_blend_state_create_info =
      vk::pipeline_color_blend_state_create_info_template;
  color_blend_state_create_info.logicOpEnable = VK_FALSE;
  color_blend_state_create_info.attachmentCount = 1;
  color_blend_state_create_info.pAttachments = &color_blend_attachment;

  VkPipelineLayoutCreateInfo pipeline_layout_create_info =
      vk::pipeline_layout_create_info_template;
  pipeline_layout_create_info.setLayoutCount = 1;
  pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout;
  pipeline_layout_create_info.pushConstantRangeCount = 0;
  pipeline_layout_create_info.pPushConstantRanges = nullptr;

  VkResult result =
      vkCreatePipelineLayout(device->GetHandle(), &pipeline_layout_create_info,
                             nullptr, &pipeline_layout);
  if (result) {
    throw vk::CriticalException("cant create pipeline layout");
  }

  TRACE("texture renderer pipeline layout created");

  VkPipelineDynamicStateCreateInfo dynamic_state =
      vk::pipeline_dynamic_state_create_info_template;
  dynamic_state.dynamicStateCount = 0;
  dynamic_state.pDynamicStates = nullptr;

  VkGraphicsPipelineCreateInfo pipeline_create_info =
      vk::graphics_pipeline_create_info_template;
  pipeline_create_info.stageCount = 2;
  pipeline_create_info.pStages = shader_stages;
  pipeline_create_info.pVertexInputState = &vertex_input;
  pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
  pipeline_create_info.pViewportState = &viewport_state_create_info;
  pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
  pipeline_create_info.pMultisampleState = &multisample_create_info;
  pipeline_create_info.pDepthStencilState = nullptr;
  pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
  pipeline_create_info.pDynamicState = &dynamic_state;
  pipeline_create_info.layout = pipeline_layout;
  pipeline_create_info.renderPass = render_pass;
  pipeline_create_info.subpass = 0;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_create_info.basePipelineIndex = -1;

  result = vkCreateGraphicsPipelines(device->GetHandle(), VK_NULL_HANDLE, 1,
                                     &pipeline_create_info, nullptr, &pipeline);
  if (result) {
    throw vk::CriticalException("cant create pipeline");
  }

  DEBUG("texture renderer graphics pipeline created");
}

void TextureRenderer::CreateCommandBuffers() {
  command_pool = make_unique<vk::CommandPool>(*device, queue, 1);

  command_buffer =
      command_pool->AllocateCommandBuffer(vk::CommandBufferLevel::primary);
}

void TextureRenderer::WriteCommandBuffer(RenderInfo &render_info) {
  command_buffer->Reset();

  command_buffer->Begin();

  VkClearValue clear_value = {{{0, 0, 0, 1}}};

  VkRenderPassBeginInfo render_pass_begin_info =
      vk::render_pass_begin_info_template;
  render_pass_begin_info.renderPass = render_info.render_pass;
  render_pass_begin_info.framebuffer = render_info.framebuffer;
  render_pass_begin_info.renderArea.offset = {0, 0};
  render_pass_begin_info.renderArea.extent = extent;
  render_pass_begin_info.clearValueCount = 1;
  render_pass_begin_info.pClearValues = &clear_value;

  VkSemaphoreWaitInfo wait_info;
  wait_info.sType
  wait_info.flags = 0;
  wait_info.pNext = 0;
  wait_info.pSemaphores = &render_info.wait_semaphore;
  wait_info.semaphoreCount = 1;
  
  vkWaitSemaphores(device->GetHandle(), 
  
  vkCmdBeginRenderPass(command_buffer->GetHandle(), &render_pass_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(command_buffer->GetHandle(),
                    VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  VkBuffer vertex_buffers[] = {vertex_buffer->GetHandle()};
  VkDeviceSize offsets[] = {0, 0};

  vkCmdBindVertexBuffers(command_buffer->GetHandle(), 0, 1, vertex_buffers,
                         offsets);

  vkCmdBindDescriptorSets(command_buffer->GetHandle(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0,
                          1, &descriptor_set, 0, nullptr);

  vkCmdDraw(command_buffer->GetHandle(), 4, 1, 0, 0);

  vkCmdEndRenderPass(command_buffer->GetHandle());

  command_buffer->End();
}

void TextureRenderer::CreateUniformBuffer() {
  vk::BufferCreateInfo create_info;
  create_info.queue = queue;
  create_info.size = sizeof(UniformData);
  create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

  uniform_buffer = make_unique<vk::Buffer>(*device, create_info);

  vector<vk::MemoryObject *> buffers = {uniform_buffer.get()};

  uint32_t memory_size = vk::DeviceMemory::CalculateMemorySize(buffers);

  vk::ChooseMemoryTypeInfo choose_info;
  choose_info.memory_types = uniform_buffer->GetMemoryTypes();
  choose_info.heap_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  choose_info.properties =
      uniform_buffer->GetMemoryRequirements().memoryTypeBits;

  uint32_t choosed_memory =
      device->GetPhysicalDevice().ChooseMemoryType(choose_info);

  uniform_buffer_memory =
      make_unique<vk::DeviceMemory>(*device, memory_size, choosed_memory);

  uniform_buffer_memory->BindBuffer(*uniform_buffer);
}

void TextureRenderer::CreateDescriptorSetLayout() {
  vector<VkDescriptorSetLayoutBinding> bindings;

  VkDescriptorSetLayoutBinding sampler_layout_binding;
  sampler_layout_binding.binding = 0;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.pImmutableSamplers = nullptr;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  bindings.push_back(sampler_layout_binding);

  VkDescriptorSetLayoutCreateInfo create_info =
      vk::descriptor_set_layout_create_info_template;
  create_info.bindingCount = bindings.size();
  create_info.pBindings = bindings.data();

  VkResult result = vkCreateDescriptorSetLayout(
      device->GetHandle(), &create_info, nullptr, &descriptor_set_layout);
  if (result) {
    throw vk::CriticalException("cant create descriptor set layout");
  }

  TRACE("texture renderer descriptor set layout created");
}

void TextureRenderer::CreateDescriptorPool() {
  vector<VkDescriptorPoolSize> pool_sizes;

  VkDescriptorPoolSize sampler_pool_size;
  sampler_pool_size.descriptorCount = 1;
  sampler_pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

  pool_sizes.push_back(sampler_pool_size);

  VkDescriptorPoolCreateInfo create_info =
      vk::descriptor_pool_create_info_template;
  create_info.poolSizeCount = pool_sizes.size();
  create_info.pPoolSizes = pool_sizes.data();
  create_info.maxSets = 1;

  VkResult result = vkCreateDescriptorPool(device->GetHandle(), &create_info,
                                           nullptr, &descriptors_pool);
  if (result) {
    throw vk::CriticalException("cant create descriptor pool");
  }

  TRACE("texture renderer descriptor pool created");
}

void TextureRenderer::AllocateDescriptorSet() {
  VkDescriptorSetAllocateInfo allocate_info =
      vk::descriptor_set_allocate_info_template;
  allocate_info.descriptorPool = descriptors_pool;
  allocate_info.descriptorSetCount = 1;
  allocate_info.pSetLayouts = &descriptor_set_layout;

  VkResult result = vkAllocateDescriptorSets(device->GetHandle(),
                                             &allocate_info, &descriptor_set);
  if (result) {
    throw vk::CriticalException("cant allocate descriptor set");
  }

  TRACE("texture renderer descriptor set allocated");
}

void TextureRenderer::UpdateDescriptorSet() {
  vector<VkWriteDescriptorSet> write_sets;

  VkDescriptorBufferInfo buffer_info;
  buffer_info.buffer = uniform_buffer->GetHandle();
  buffer_info.offset = 0;
  buffer_info.range = VK_WHOLE_SIZE;

  VkWriteDescriptorSet ubo_write_set = vk::write_descriptor_set_template;
  ubo_write_set.dstSet = descriptor_set;
  ubo_write_set.dstBinding = 0;
  ubo_write_set.dstArrayElement = 0;
  ubo_write_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_write_set.descriptorCount = 1;
  ubo_write_set.pBufferInfo = &buffer_info;

  write_sets.push_back(ubo_write_set);

  VkDescriptorImageInfo image_info;
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView = texture_view->GetHandle();
  image_info.sampler = texture_sampler;

  VkWriteDescriptorSet texture_write_set = vk::write_descriptor_set_template;
  texture_write_set.dstSet = descriptor_set;
  texture_write_set.dstBinding = 1;
  texture_write_set.dstArrayElement = 0;
  texture_write_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  texture_write_set.descriptorCount = 1;
  texture_write_set.pImageInfo = &image_info;

  write_sets.push_back(texture_write_set);

  vkUpdateDescriptorSets(device->GetHandle(), write_sets.size(),
                         write_sets.data(), 0, nullptr);

  TRACE("texture renderer descriptors set updated");
}

void TextureRenderer::CreateTextureSampler() {
  VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  VkSamplerCreateInfo create_info = vk::sampler_create_info_template;
  create_info.addressModeU = address_mode;
  create_info.addressModeV = address_mode;
  create_info.addressModeW = address_mode;

  VkResult result = vkCreateSampler(device->GetHandle(), &create_info, nullptr,
                                    &texture_sampler);
  if (result) {
    throw vk::CriticalException("cant create texture sampler");
  }
}

void TextureRenderer::CreateVertexBuffer() {
  SpriteVertex vertices[] = {
      {{-1, -1}, {}}, {{-1, 1}, {}}, {{1, 1}, {}}, {{1, -1}, {}}};

  vk::BufferCreateInfo create_info;
  create_info.queue = queue;
  create_info.size = sizeof(vertices);
  create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

  vertex_buffer = make_unique<vk::Buffer>(*device, create_info);

  vk::PhysicalDevice &physical_device = device->GetPhysicalDevice();

  vk::ChooseMemoryTypeInfo choose_info;
  choose_info.memory_types = vertex_buffer->GetMemoryTypes();
  choose_info.heap_properties = 0;
  choose_info.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

  uint32_t memory_type = physical_device.ChooseMemoryType(choose_info);

  vector<vk::MemoryObject *> buffers = {vertex_buffer.get()};

  VkDeviceSize memory_size = vk::DeviceMemory::CalculateMemorySize(buffers);

  vertex_buffer_memory =
      make_unique<vk::DeviceMemory>(*device, memory_size, memory_type);

  vertex_buffer_memory->BindBuffer(*vertex_buffer);

  void *mapped_data = vertex_buffer->Map();

  memcpy(mapped_data, vertices, sizeof(vertices));

  vertex_buffer->Flush();
  vertex_buffer->Unmap();

  TRACE("texture renderer vertex buffer created");
}
