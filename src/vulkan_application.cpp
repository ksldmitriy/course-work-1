#include "vulkan_application.hpp"

VulkanApplication::VulkanApplication() {}

VulkanApplication::~VulkanApplication() {

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

  CreateSyncObjects();

  CreateTextureRenderPass();
  
  CreateFramebuffers();

  DEBUG("vulkan application prepared");
}

void VulkanApplication::CreateSyncObjects() {
  VkSemaphoreCreateInfo semphore_create_info =
      vk::semaphore_create_info_template;

  VkFenceCreateInfo fence_create_info = vk::fence_create_info_template;

  VkResult result = vkCreateFence(device->GetHandle(), &fence_create_info, nullptr, &fence);
  if(result){
	throw vk::CriticalException("cant create fence");
  }
  
  TRACE("fence created");
}

void VulkanApplication::CreateTextureRenderer(){
  TextureRendererCreateInfo create_info;
  create_info.device = device.get();
  create_info.queue = graphics_queue;
  create_info.framebuffers = framebuffers;
  create_info.render_pass = pheromone_render_pass;
  create_info.extent = swapchain->GetExtent();
  create_info.texture_view = pheromone_map_view.get();
}

void VulkanApplication::CleanupSyncObjects() {

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

  surface_changed = true;
}

void VulkanApplication::CreatePheromoneMap() {
  vk::ImageCreateInfo create_info;
  create_info.format = VK_FORMAT_R32_SFLOAT;
  create_info.layout = VK_IMAGE_LAYOUT_UNDEFINED;
  create_info.size = map_size;

  pheromone_map_image = make_unique<vk::Image>(device.get(), create_info);

  pheromone_map_view = make_unique<vk::ImageView>(device.get(), pheromone_map_image.get());
  
}

void VulkanApplication::CreateTextureRenderPass() {
  VkAttachmentDescription surface_attachment =
      vk::attachment_description_template;
  surface_attachment.format = swapchain->GetFormat().format;
  surface_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  surface_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  surface_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  surface_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_reference;
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass_description = vk::subpass_description_template;
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_reference;

  VkRenderPassCreateInfo create_info = vk::render_pass_create_info_template;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &surface_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass_description;

  VkResult result = vkCreateRenderPass(device->GetHandle(), &create_info,
                                       nullptr, &pheromone_render_pass);
  if (result) {
    throw vk::CriticalException("cant create render pass");
  }

  DEBUG("pheromone pass created");
}

bool VulkanApplication::IsSurfaceChanged() {
  return surface_changed;
  surface_changed = false;
}

void VulkanApplication::Draw() {}

void VulkanApplication::Render(uint32_t next_image_index) {}

void VulkanApplication::Present(uint32_t next_image_index) {}

void VulkanApplication::CreateFramebuffers() {
  VkFramebufferCreateInfo create_info = vk::framebuffer_create_info_template;
  create_info.renderPass = pheromone_render_pass;
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
