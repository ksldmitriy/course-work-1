#include "imgui_renderer.hpp"

bool ImguiRenderer::imgui_initied = false;

ImguiRenderer::ImguiRenderer(ImguiRendererCreateInfo &create_info) {
  instance = create_info.instance;
  device = create_info.device;
  queue = create_info.queue;

  window = create_info.window;

  swapchain_min_image_count = create_info.swapchain_min_image_count;
  extent = create_info.extent;
  render_pass = create_info.render_pass;
  framebuffers = create_info.framebuffers;

  Init();
}

ImguiRenderer::~ImguiRenderer() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  one_time_command_buffer->Dispose();
  render_command_buffer->Dispose();
  imgui_command_pool->Dispose();

  vkDestroyDescriptorPool(device->GetHandle(), imgui_descriptor_pool, nullptr);
}

void ImguiRenderer::Init() {
  InitImgui();

  DEBUG("imgui renderer inited");
}

void ImguiRenderer::InitImgui() {
  imgui_command_pool = make_unique<vk::CommandPool>(*device, queue, 2);
  one_time_command_buffer = imgui_command_pool->AllocateCommandBuffer(
      vk::CommandBufferLevel::primary);
  render_command_buffer = imgui_command_pool->AllocateCommandBuffer(
      vk::CommandBufferLevel::primary);

  ImGui::CreateContext();
  //  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags;

  window->AttachImguiIO(&ImGui::GetIO());
  
  
  ImGui_ImplGlfw_InitForVulkan(window->GetHandle(), true);

  CreateImguiDescriptorPool();

  ImGui_ImplVulkan_InitInfo init_info;
  init_info.Instance = instance->GetHandle();
  init_info.PhysicalDevice = device->GetPhysicalDevice().GetHandle();
  init_info.Device = device->GetHandle();
  init_info.QueueFamily = queue.GetFamily();
  init_info.Queue = queue.GetHandle();
  init_info.PipelineCache = nullptr;
  init_info.DescriptorPool = imgui_descriptor_pool;
  init_info.Subpass = 0;
  init_info.MinImageCount = swapchain_min_image_count;
  init_info.ImageCount = framebuffers.size();
  init_info.Allocator = nullptr;
  init_info.CheckVkResultFn = nullptr;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

  bool result = ImGui_ImplVulkan_Init(&init_info, render_pass);
  if (!result) {
    throw CriticalException("cant init imgui vulkan");
  }

  one_time_command_buffer->Begin();
  ImGui_ImplVulkan_CreateFontsTexture(one_time_command_buffer->GetHandle());
  one_time_command_buffer->End();
  one_time_command_buffer->SoloExecute();
  one_time_command_buffer->Reset();

  ImGui_ImplVulkan_DestroyFontUploadObjects();

  DEBUG("imgui inited");
}

void ImguiRenderer::CreateImguiDescriptorPool() {
  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  VkDescriptorPoolCreateInfo pool_info =
      vk::descriptor_pool_create_info_template;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
  pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;

  VkResult result = vkCreateDescriptorPool(device->GetHandle(), &pool_info,
                                           nullptr, &imgui_descriptor_pool);
  if (result) {
    throw vk::CriticalException("cant create descriptor pool for imgui");
  }

  DEBUG("imgui descriptor pool created");
}

void ImguiRenderer::CreateCommandBuffer(uint32_t image_index) {
  render_command_buffer->Begin();

  VkClearValue clear_value = {{{0, 0, 0, 1}}};

  VkRenderPassBeginInfo render_pass_begin_info =
      vk::render_pass_begin_info_template;
  render_pass_begin_info.renderPass = render_pass;
  render_pass_begin_info.framebuffer = framebuffers[image_index];
  render_pass_begin_info.renderArea.offset = {0, 0};
  render_pass_begin_info.renderArea.extent = extent;
  render_pass_begin_info.clearValueCount = 1;
  render_pass_begin_info.pClearValues = &clear_value;

  vkCmdBeginRenderPass(render_command_buffer->GetHandle(),
                       &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                  render_command_buffer->GetHandle());

  vkCmdEndRenderPass(render_command_buffer->GetHandle());

  render_command_buffer->End();
}

void ImguiRenderer::Render(uint32_t image_index,
                           VkSemaphore image_available_semaphore,
                           VkSemaphore render_finished_semaphore,
                           VkFence fence) {
  CreateCommandBuffer(image_index);

  VkCommandBuffer command_buffer_handle = render_command_buffer->GetHandle();

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
