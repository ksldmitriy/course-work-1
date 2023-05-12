#include "instance_renderer.hpp"

InstanceRenderer::InstanceRenderer(InstanceRendererCreateInfo &create_info) {
  device = create_info.device;
  queue = create_info.queue;

  framebuffers = create_info.framebuffers;
  extent = create_info.extent;
  render_pass = create_info.render_pass;

  DEBUG("instance renderer created");
}

void InstanceRenderer::Init() {
  CreateVertexInputBuffers();
  CreateUniformBuffer();

  CreateDescriptorSetLayout();
  CreateDescriptorPool();
  AllocateDescriptorSet();
  UpdateDescriptorSet();

  CreatePipeline(extent, render_pass);

  CreateCommandBuffer();

  DEBUG("instance renderer inited");
}

InstanceRenderer::~InstanceRenderer() {
  for (int i = 0; i < command_buffers.size(); i++) {
    command_buffers[i]->Dispose();
  }

  command_pool->Dispose();

  vertex_buffer->Destroy();
  instance_buffer->Destroy();
  uniform_buffer->Destroy();

  vertex_buffer_memory->Free();
  uniform_buffer_memory->Free();

  vkDestroyDescriptorSetLayout(device->GetHandle(), descriptor_set_layout, nullptr);

  vkDestroyDescriptorPool(device->GetHandle(), descriptors_pool, nullptr);
  
  vkDestroyPipelineLayout(device->GetHandle(), pipeline_layout, nullptr);
  
  vkDestroyPipeline(device->GetHandle(), pipeline,  nullptr);
  
  DEBUG("instance renderer destroyed");
}

void InstanceRenderer::Render(uint32_t image_index,
                              VkSemaphore image_available_semaphore,
                              VkSemaphore render_finished_semaphore,
                              VkFence fence) {
  VkCommandBuffer command_buffer_handle =
      command_buffers[image_index]->GetHandle();

  VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

  VkSubmitInfo submit_info = vk::submit_info_template;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer_handle;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &image_available_semaphore;
  submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &render_finished_semaphore;

  VkResult result = vkQueueSubmit(queue.GetHandle(), 1, &submit_info, fence);
  if (result) {
    throw vk::CriticalException("cant submit to queue");
  }
}

void InstanceRenderer::CreateCommandBuffer() {
  command_pool =
      make_unique<vk::CommandPool>(*device, queue, framebuffers.size());

  command_buffers.resize(framebuffers.size());

  for (int i = 0; i < framebuffers.size(); i++) {
    unique_ptr<vk::CommandBuffer> command_buffer =
        command_pool->AllocateCommandBuffer(vk::CommandBufferLevel::primary);

    command_buffer->Begin();

    VkClearValue clear_value = {{{0, 0, 0, 1}}};

    VkRenderPassBeginInfo render_pass_begin_info =
        vk::render_pass_begin_info_template;
    render_pass_begin_info.renderPass = render_pass;
    render_pass_begin_info.framebuffer = framebuffers[i];
    render_pass_begin_info.renderArea.offset = {0, 0};
    render_pass_begin_info.renderArea.extent = extent;
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_value;

    vkCmdBeginRenderPass(command_buffer->GetHandle(), &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffer->GetHandle(),
                      VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkBuffer vertex_buffers[] = {vertex_buffer->GetHandle(),
                                 instance_buffer->GetHandle()};
    VkDeviceSize offsets[] = {0, 0};

    vkCmdBindVertexBuffers(command_buffer->GetHandle(), 0, 2, vertex_buffers,
                           offsets);

    vkCmdBindDescriptorSets(command_buffer->GetHandle(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0,
                            1, &descriptor_set, 0, nullptr);

    vkCmdDraw(command_buffer->GetHandle(), 3, 1, 0, 0);

    vkCmdEndRenderPass(command_buffer->GetHandle());

    command_buffer->End();

    command_buffers[i] = move(command_buffer);
  }

  TRACE("render command buffers created");
}

void InstanceRenderer::CreatePipeline(VkExtent2D extent,
                                      VkRenderPass render_pass) {
  unique_ptr<vk::ShaderModule> vertex_shader =
      make_unique<vk::ShaderModule>(*device, "shaders/vert.spv");
  unique_ptr<vk::ShaderModule> fragment_shader =
      make_unique<vk::ShaderModule>(*device, "shaders/frag.spv");

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
  auto vertex_binding_description = Vertex::GetBindingDescription(0);
  auto zalupa_binding_description = InstanceData::GetBindingDescription(1);

  binding_description.push_back(vertex_binding_description);
  binding_description.push_back(zalupa_binding_description);

  vector<VkVertexInputAttributeDescription> attribute_descriptions;
  auto vertex_attribute_descriptions = Vertex::GetAttributeDescriptions(0, 0);
  auto zalupa_attribute_descriptions =
      InstanceData::GetAttributeDescriptions(1, 2);

  attribute_descriptions.insert(attribute_descriptions.end(),
                                vertex_attribute_descriptions.begin(),
                                vertex_attribute_descriptions.end());

  attribute_descriptions.insert(attribute_descriptions.end(),
                                zalupa_attribute_descriptions.begin(),
                                zalupa_attribute_descriptions.end());

  VkPipelineVertexInputStateCreateInfo vertex_input =
      vk::vertex_input_create_info_template;
  vertex_input.vertexBindingDescriptionCount = binding_description.size();
  vertex_input.pVertexBindingDescriptions = binding_description.data();
  vertex_input.vertexAttributeDescriptionCount = attribute_descriptions.size();
  vertex_input.pVertexAttributeDescriptions = attribute_descriptions.data();

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

  TRACE("pipeline layout created");

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

  DEBUG("graphics pipeline created");
}

void InstanceRenderer::CreateUniformBuffer() {
  vk::BufferCreateInfo create_info;
  create_info.queue = queue;
  create_info.size = sizeof(UniformData);
  create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

  uniform_buffer = make_unique<vk::Buffer>(*device, create_info);

  vector<vk::Buffer *> buffers = {uniform_buffer.get()};
  uint32_t uniform_buffer_memory_size =
      vk::DeviceMemory::CalculateMemorySize(buffers);

  vk::PhysicalDevice &physical_device = device->GetPhysicalDevice();

  vk::ChooseMemoryTypeInfo choose_info;
  choose_info.memory_types = uniform_buffer->GetMemoryTypes();
  choose_info.heap_properties = 0;
  choose_info.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

  uint32_t memory_type = physical_device.ChooseMemoryType(choose_info);

  uniform_buffer_memory = make_unique<vk::DeviceMemory>(
      *device, uniform_buffer->GetSize(), memory_type);

  uniform_buffer_memory->BindBuffer(*uniform_buffer);

  UniformData *uniform_data = (UniformData *)uniform_buffer->Map();

  uniform_data->scale = {1, 1};

  uniform_buffer->Flush();
  uniform_buffer->Unmap();
}

void InstanceRenderer::CreateVertexInputBuffers() {
  Vertex vertices[3] = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  for (int i = 0; i < 3; i++) {
    double angle = i * -3.14 / 1.5;
    glm::fvec2 pos = {sin(angle), -cos(angle)};
    vertices[i].pos = pos / 6.0f;
  }

  // create buffers
  vk::BufferCreateInfo create_info;
  create_info.queue = queue;
  create_info.size = sizeof(vertices);
  create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

  vertex_buffer = make_unique<vk::Buffer>(*device, create_info);

  create_info.size = sizeof(InstanceData);

  instance_buffer = make_unique<vk::Buffer>(*device, create_info);

  create_info.size = sizeof(UniformData);
  create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

  // allocate memory
  vk::PhysicalDevice &physical_device = device->GetPhysicalDevice();

  vk::ChooseMemoryTypeInfo choose_info;
  choose_info.memory_types = vertex_buffer->GetMemoryTypes();
  choose_info.heap_properties = 0;
  choose_info.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

  uint32_t memory_type = physical_device.ChooseMemoryType(choose_info);

  vector<vk::Buffer *> buffers = {vertex_buffer.get(), instance_buffer.get()};

  VkDeviceSize memory_size = vk::DeviceMemory::CalculateMemorySize(buffers);

  vertex_buffer_memory =
      make_unique<vk::DeviceMemory>(*device, memory_size, memory_type);

  vertex_buffer_memory->BindBuffer(*vertex_buffer);
  vertex_buffer_memory->BindBuffer(*instance_buffer);

  TRACE("vertex buffer created and binded to memory");

  // load data
  char *mapped_memory = (char *)vertex_buffer->Map();

  memcpy(mapped_memory, (char *)vertices, sizeof(vertices));

  vertex_buffer->Flush();
  vertex_buffer->Unmap();

  TRACE("data loaded to vertex buffers");
}

void InstanceRenderer::UpdateDescriptorSet() {
  VkDescriptorBufferInfo buffer_info;
  buffer_info.buffer = uniform_buffer->GetHandle();
  buffer_info.offset = 0;
  buffer_info.range = VK_WHOLE_SIZE;

  VkWriteDescriptorSet write_set = vk::write_descriptor_set_template;
  write_set.dstSet = descriptor_set;
  write_set.dstBinding = 0;
  write_set.dstArrayElement = 0;
  write_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  write_set.descriptorCount = 1;
  write_set.pBufferInfo = &buffer_info;

  vkUpdateDescriptorSets(device->GetHandle(), 1, &write_set, 0, nullptr);

  TRACE("descriptors set updated");
}

void InstanceRenderer::AllocateDescriptorSet() {
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

  TRACE("descriptor set allocated");
}

void InstanceRenderer::CreateDescriptorPool() {
  VkDescriptorPoolSize pool_size;
  pool_size.descriptorCount = 1;
  pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

  VkDescriptorPoolCreateInfo create_info =
      vk::descriptor_pool_create_info_template;
  create_info.poolSizeCount = 1;
  create_info.pPoolSizes = &pool_size;
  create_info.maxSets = 1;

  VkResult result = vkCreateDescriptorPool(device->GetHandle(), &create_info,
                                           nullptr, &descriptors_pool);
  if (result) {
    throw vk::CriticalException("cant create descriptor pool");
  }

  TRACE("descriptor pool created");
}

void InstanceRenderer::CreateDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding ubo_layout_binding;
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  ubo_layout_binding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo create_info =
      vk::descriptor_set_layout_create_info_template;
  create_info.bindingCount = 1;
  create_info.pBindings = &ubo_layout_binding;

  VkResult result = vkCreateDescriptorSetLayout(
      device->GetHandle(), &create_info, nullptr, &descriptor_set_layout);
  if (result) {
    throw vk::CriticalException("cant create descriptor set layout");
  }

  TRACE("descriptor set layout created");
}
