#include "vulkan_application.hpp"

VulkanApplication::VulkanApplication() {}

VulkanApplication::~VulkanApplication() {

  staging_command_buffer->Dispose();

  command_pool->Dispose();

  DestoyRenderers();

  vkDestroyRenderPass(device->GetHandle(), road_render_pass, nullptr);
  vkDestroyRenderPass(device->GetHandle(), cars_render_pass, nullptr);
  vkDestroyRenderPass(device->GetHandle(), debug_render_pass, nullptr);
  vkDestroyRenderPass(device->GetHandle(), imgui_render_pass, nullptr);

  CleanupFramebuffers();

  CleanupSyncObjects();

  car_texture->Destroy();

  swapchain->Dispose();

  window->Destroy();

  device->Dispose();

  instance->Dispose();

  INFO("vulkan application destroyed");
}

void VulkanApplication::InitVulkan(uint32_t glfw_extensions_count,
                                   const char **glfw_extensions) {
  CreateInstance(glfw_extensions_count, glfw_extensions);

  CreateDevice();

  DEBUG("vulkan inited");
}

void VulkanApplication::Prepare() {
  window = unique_ptr<Window>(new Window());

  uint32_t glfw_extensions_count;
  const char **glfw_extensions;

  window->GetInstanceExtensions(glfw_extensions, glfw_extensions_count);

  InitVulkan(glfw_extensions_count, glfw_extensions);

  window->AttachInstance(*instance);

  window->CreateSurface();

  swapchain = make_unique<vk::Swapchain>(*device, window->GetSurface());

  CreateRoadRenderPass();
  CreateCarsRenderPass();
  CreateDebugRenderPass();
  CreateImguiRenderPass();

  CreateSyncObjects();

  CreateFramebuffers();

  command_pool = make_unique<vk::CommandPool>(*device, graphics_queue, 1);

  staging_command_buffer =
      command_pool->AllocateCommandBuffer(vk::CommandBufferLevel::primary);

  CreateTextures();

  CreateRenderers();

  DEBUG("vulkan application prepared");
}

void VulkanApplication::CreateRenderers() {
  CreateRoadRenderer();
  CreateCarsRenderer();
  CreateDebugRenderer();
  CreateImguiRenderer();
}

void VulkanApplication::DestoyRenderers() {
  road_renderer.reset();
  cars_renderer.reset();
  debug_renderer.reset();
  imgui_renderer.reset();
}

void VulkanApplication::CreateTextures() {
  fs::path image_path = "car-texture.png";

  glm::ivec2 image_size;
  int image_channels;

  stbi_uc *image_data =
      stbi_load(image_path.c_str(), &image_size.x, &image_size.y,
                &image_channels, STBI_rgb_alpha);
  if (!image_data) {
    throw CriticalException("cant load texture " + image_path.string());
  }

  car_texture = make_unique<vk::Texture>(device.get());
  car_texture->LoadImage((char *)image_data, image_size,
                         staging_command_buffer.get(), graphics_queue);

  stbi_image_free(image_data);

  DEBUG("textures created");
}

void VulkanApplication::CreateSyncObjects() {
  VkSemaphoreCreateInfo semphore_create_info =
      vk::semaphore_create_info_template;

  VkFenceCreateInfo fence_create_info = vk::fence_create_info_template;

  image_available_semaphore = make_unique<vk::Semaphore>(device.get());
  road_render_finished_semaphore = make_unique<vk::Semaphore>(device.get());
  cars_render_finished_semaphore = make_unique<vk::Semaphore>(device.get());
  debug_render_finished_semaphore = make_unique<vk::Semaphore>(device.get());
  imgui_render_finished_semaphore = make_unique<vk::Semaphore>(device.get());

  VkResult result =
      vkCreateFence(device->GetHandle(), &fence_create_info, nullptr, &fence);
  if (result) {
    throw vk::CriticalException("cant create fence");
  }

  TRACE("image available semaphore created");
  TRACE("render finished semaphore created");
  TRACE("fence created");
}

void VulkanApplication::CleanupSyncObjects() {
  image_available_semaphore.reset();
  road_render_finished_semaphore.reset();
  cars_render_finished_semaphore.reset();
  debug_render_finished_semaphore.reset();
  imgui_render_finished_semaphore.reset();

  vkDestroyFence(device->GetHandle(), fence, nullptr);
}

void VulkanApplication::ChangeSurface() {
  DEBUG("changing suface");

  vkDeviceWaitIdle(device->GetHandle());

  swapchain.reset();

  window->DestroySurface();
  window->CreateSurface();

  swapchain = make_unique<vk::Swapchain>(*device, window->GetSurface());

  CleanupSyncObjects();
  CreateSyncObjects();

  CleanupFramebuffers();
  CreateFramebuffers();

  DestoyRenderers();
  CreateRenderers();

  surface_changed = true;
}


bool VulkanApplication::IsSurfaceChanged(){
  return surface_changed;
  surface_changed = false;
}


void VulkanApplication::CreateImguiRenderer() {
  imgui_renderer.reset();

  ImguiRendererCreateInfo create_info;
  create_info.instance = instance.get();
  create_info.device = device.get();
  create_info.queue = graphics_queue;
  create_info.framebuffers = framebuffers;
  create_info.extent = swapchain->GetExtent();
  create_info.swapchain_min_image_count = swapchain->GetMinImageCount();
  create_info.window = window.get();
  create_info.render_pass = imgui_render_pass;

  imgui_renderer = make_unique<ImguiRenderer>(create_info);
}

void VulkanApplication::CreateRoadRenderer() {
  MeshRendererSettings settings;
  settings.camera = {{0, 0}, 1};

  MeshRendererCreateInfo create_info;
  create_info.device = device.get();
  create_info.queue = graphics_queue;
  create_info.framebuffers = framebuffers;
  create_info.extent = swapchain->GetExtent();
  create_info.render_pass = road_render_pass;
  create_info.settings = settings;

  road_renderer = make_unique<MeshRenderer>(create_info);
}

void VulkanApplication::CreateDebugRenderer() {
  DebugRendererSettings settings;
  settings.camera = {{0, 0}, 1};

  DebugRendererCreateInfo create_info;
  create_info.device = device.get();
  create_info.queue = graphics_queue;
  create_info.framebuffers = framebuffers;
  create_info.extent = swapchain->GetExtent();
  create_info.render_pass = debug_render_pass;
  create_info.settings = settings;

  debug_renderer = make_unique<DebugRenderer>(create_info);
}

void VulkanApplication::CreateCarsRenderer() {
  const int total_sprites = 7;

  InstanceRendererSettings settings;
  settings.camera = {{0, 0}, 1};
  settings.sprite_size = glm::fvec2(0.56, 1) / 3.0f;

  InstanceRendererCreateInfo create_info;
  create_info.device = device.get();
  create_info.queue = graphics_queue;
  create_info.framebuffers = framebuffers;
  create_info.extent = swapchain->GetExtent();
  create_info.render_pass = cars_render_pass;
  create_info.texture = car_texture->CreateImageView();
  create_info.settings = settings;

  cars_renderer = make_unique<InstanceRenderer>(create_info);
}

void VulkanApplication::Draw() {
  try {
    uint32_t next_image_index =
        swapchain->AcquireNextImage(image_available_semaphore->GetHandle());

    Render(next_image_index);

    Present(next_image_index);

    vkWaitForFences(device->GetHandle(), 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device->GetHandle(), 1, &fence);
  } catch (vk::AcquireNextImageFailedException e) {
    ChangeSurface();
  } catch (vk::PresentFailedException e) {
    ChangeSurface();
  }
}

void VulkanApplication::Render(uint32_t next_image_index) {
  road_renderer->Render(
      next_image_index, image_available_semaphore->GetHandle(),
      road_render_finished_semaphore->GetHandle(), VK_NULL_HANDLE);

  cars_renderer->Render(
      next_image_index, road_render_finished_semaphore->GetHandle(),
      cars_render_finished_semaphore->GetHandle(), VK_NULL_HANDLE);

  debug_renderer->Render(
      next_image_index, cars_render_finished_semaphore->GetHandle(),
      debug_render_finished_semaphore->GetHandle(), VK_NULL_HANDLE);

  imgui_renderer->Render(next_image_index,
                         debug_render_finished_semaphore->GetHandle(),
                         imgui_render_finished_semaphore->GetHandle(), fence);
}

void VulkanApplication::Present(uint32_t next_image_index) {
  VkSwapchainKHR swapchain_handle = swapchain->GetHandle();

  VkPresentInfoKHR present_info = vk::present_info_template;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &imgui_render_finished_semaphore->GetHandle();
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swapchain_handle;
  present_info.pImageIndices = &next_image_index;

  VkResult result =
      vkQueuePresentKHR(graphics_queue.GetHandle(), &present_info);
  if (result) {
    throw vk::PresentFailedException();
  }
}


void VulkanApplication::SetCamera(Camera camera){
  road_renderer->SetCamera(camera);
  debug_renderer->SetCamera(camera);
  cars_renderer->SetCamera(camera);
}

void VulkanApplication::CreateFramebuffers() {
  VkFramebufferCreateInfo create_info = vk::framebuffer_create_info_template;
  create_info.renderPass = cars_render_pass;
  create_info.attachmentCount = 1;
  create_info.width = swapchain->GetExtent().width;
  create_info.height = swapchain->GetExtent().height;
  create_info.layers = 1;

  const vector<VkImageView> image_views = swapchain->GetImageViews();
  framebuffers.resize(image_views.size());

  for (int i = 0; i < framebuffers.size(); i++) {
    create_info.pAttachments = &image_views[i];
    VkResult result = vkCreateFramebuffer(device->GetHandle(), &create_info,
                                          nullptr, &framebuffers[i]);
    if (result) {
      throw vk::CriticalException("cant create framebuffer");
    }
  }

  DEBUG("framebuffers created");
}

void VulkanApplication::CleanupFramebuffers() {
  for (int i = 0; i < framebuffers.size(); i++) {
    vkDestroyFramebuffer(device->GetHandle(), framebuffers[i], nullptr);
  }
}

void VulkanApplication::CreateImguiRenderPass() {
  VkAttachmentDescription color_attachment =
      vk::attachment_description_template;
  color_attachment.format = swapchain->GetFormat().format;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_reference;
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass_description = vk::subpass_description_template;
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_reference;

  VkRenderPassCreateInfo create_info = vk::render_pass_create_info_template;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &color_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass_description;

  VkResult result = vkCreateRenderPass(device->GetHandle(), &create_info,
                                       nullptr, &imgui_render_pass);
  if (result) {
    throw vk::CriticalException("cant create render pass");
  }

  DEBUG("imgui render pass created");
}

void VulkanApplication::CreateDebugRenderPass() {
  VkAttachmentDescription color_attachment =
      vk::attachment_description_template;
  color_attachment.format = swapchain->GetFormat().format;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_reference;
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass_description = vk::subpass_description_template;
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_reference;

  VkRenderPassCreateInfo create_info = vk::render_pass_create_info_template;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &color_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass_description;

  VkResult result = vkCreateRenderPass(device->GetHandle(), &create_info,
                                       nullptr, &debug_render_pass);
  if (result) {
    throw vk::CriticalException("cant create render pass");
  }

  DEBUG("debug render pass created");
}

void VulkanApplication::CreateCarsRenderPass() {
  VkAttachmentDescription color_attachment =
      vk::attachment_description_template;
  color_attachment.format = swapchain->GetFormat().format;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_reference;
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass_description = vk::subpass_description_template;
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_reference;

  VkRenderPassCreateInfo create_info = vk::render_pass_create_info_template;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &color_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass_description;

  VkResult result = vkCreateRenderPass(device->GetHandle(), &create_info,
                                       nullptr, &cars_render_pass);
  if (result) {
    throw vk::CriticalException("cant create render pass");
  }

  DEBUG("cars render pass created");
}

void VulkanApplication::CreateRoadRenderPass() {
  VkAttachmentDescription color_attachment =
      vk::attachment_description_template;
  color_attachment.format = swapchain->GetFormat().format;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_reference;
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass_description = vk::subpass_description_template;
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_reference;

  VkRenderPassCreateInfo create_info = vk::render_pass_create_info_template;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &color_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass_description;

  VkResult result = vkCreateRenderPass(device->GetHandle(), &create_info,
                                       nullptr, &road_render_pass);
  if (result) {
    throw vk::CriticalException("cant create render pass");
  }

  DEBUG("road render pass created");
}

void VulkanApplication::CreateInstance(uint32_t glfw_extensions_count,
                                       const char **glfw_extensions) {
  vk::InstanceCreateInfo create_info;

  create_info.layers.push_back("VK_LAYER_KHRONOS_validation");
  create_info.extensions.push_back("VK_EXT_debug_utils");

  for (int i = 0; i < glfw_extensions_count; i++) {
    create_info.extensions.push_back(glfw_extensions[i]);
  }

  instance = unique_ptr<vk::Instance>(new vk::Instance(create_info));
}

void VulkanApplication::CreateDevice() {
  shared_ptr<vk::PhysicalDevice> physical_device =
      instance->physical_devices[0];

  vk::DeviceCreateInfo::QueueRequest graphics_queue_request;
  graphics_queue_request.flags = VK_QUEUE_GRAPHICS_BIT;
  graphics_queue_request.queue = &graphics_queue;

  vk::DeviceCreateInfo create_info;
  create_info.queue_requests.push_back(graphics_queue_request);

  device = unique_ptr<vk::Device>(new vk::Device(physical_device, create_info));
}
