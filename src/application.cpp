#include "application.hpp"

Application ::~Application() {
  staging_command_buffer->Dispose();

  command_pool->Dispose();

  instance_renderer.reset();

  vkDestroyRenderPass(device->GetHandle(), render_pass, nullptr);

  CleanupFramebuffers();

  CleanupSyncObjects();

  swapchain->Dispose();

  car_image->Destroy();

  car_image_memory->Free();

  device->Dispose();

  INFO("application destroyed");
}

void Application::Run() {
  window = unique_ptr<Window>(new Window());

  InitVulkan();

  window->AttachInstance(*instance);

  window->CreateSurface();

  swapchain = make_unique<vk::Swapchain>(*device, window->GetSurface());

  Prepare();

  RenderLoop();
}

void Application::InitVulkan() {
  CreateInstance();

  CreateDevice();

  DEBUG("vulkan inited");
}

void Application::Prepare() {
  CreateRenderPass();
  CreateSyncObjects();

  CreateFramebuffers();

  command_pool = make_unique<vk::CommandPool>(*device, graphics_queue, 1);

  staging_command_buffer =
      command_pool->AllocateCommandBuffer(vk::CommandBufferLevel::primary);

  CreateTextures();

  CreateInstanceRenderer();

  program_start = now();

  DEBUG("application prepared");
}

void Application::CreateTextures() {
  // load car texture from file
  fs::path image_path = "car-texture.png";

  glm::ivec2 image_size;
  int image_channels;
  stbi_uc *image_data =
      stbi_load(image_path.c_str(), &image_size.x, &image_size.y,
                &image_channels, STBI_rgb_alpha);

  int image_size_in_bytes = image_size.x * image_size.y * 4;

  if (!image_data) {
    throw CriticalException("cant load texture " + image_path.string());
  }

  // create car image and its memory
  vk::ImageCreateInfo image_crate_info;
  image_crate_info.size = image_size;
  image_crate_info.format = VK_FORMAT_R8G8B8A8_SRGB;

  car_image = make_unique<vk::Image>(*device, image_crate_info);

  vector<vk::MemoryObject *> memory_objects = {car_image.get()};
  VkDeviceSize memory_size =
      vk::DeviceMemory::CalculateMemorySize(memory_objects);

  vk::PhysicalDevice &physical_device = device->GetPhysicalDevice();

  vk::ChooseMemoryTypeInfo choose_info;
  choose_info.memory_types = car_image->GetMemoryTypes();
  choose_info.heap_properties = 0;
  choose_info.properties = 0;

  uint32_t memory_type = physical_device.ChooseMemoryType(choose_info);

  car_image_memory =
      make_unique<vk::DeviceMemory>(*device, memory_size, memory_type);

  car_image_memory->BindImage(*car_image);

  // load data to image
  vk::StagingBufferCreateInfo staging_buffer_create_info;
  staging_buffer_create_info.command_buffer = staging_command_buffer.get();
  staging_buffer_create_info.queue = graphics_queue;
  staging_buffer_create_info.size = image_size_in_bytes;

  unique_ptr<vk::StagingBuffer> staging_buffer =
      make_unique<vk::StagingBuffer>(*device, staging_buffer_create_info);

  staging_command_buffer->Begin();

  staging_buffer->LoadData(span<char>((char *)image_data, image_size_in_bytes));

  stbi_image_free(image_data);

  vk::SrcImageBarrier src_barrier;

  vk::SrcImageBarrier afterload_src_barrier =
      staging_buffer->CopyToImage(car_image.get(), src_barrier);

  car_image->ChangeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vk::DstImageBarrier afterload_dst_barrier;
  afterload_dst_barrier.access = VK_ACCESS_SHADER_READ_BIT;
  afterload_dst_barrier.layout = car_image->GetLayout();
  afterload_dst_barrier.stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

  vk::ImageBarrier afterload_barrier(*car_image, afterload_src_barrier,
                                     afterload_dst_barrier);
  afterload_barrier.Set(*staging_command_buffer);

  staging_command_buffer->End();

  staging_command_buffer->SoloExecute();

  DEBUG("textures created");
}

void Application::CreateInstanceRenderer() {
  const int total_sprites = 7;

  InstanceRendererCreateInfo create_info;
  create_info.device = device.get();
  create_info.queue = graphics_queue;
  create_info.framebuffers = framebuffers;
  create_info.extent = swapchain->GetExtent();
  create_info.render_pass = render_pass;
  create_info.texture = car_image.get();

  create_info.sprite_size = glm::fvec2(0.56, 1) / 3.0f;

  instance_renderer = make_unique<InstanceRenderer>(create_info);

  vector<Transforn2D> sprites(total_sprites);
  
  for (int i = 0; i < total_sprites; i++) {
    Transforn2D sprite;
    sprite.pos.x = -1 + (2.0 / total_sprites) * i;
    sprite.pos.y = 0;
    sprite.rot = 0;

    sprites[i] = sprite;
  }

  instance_renderer->LoadSprites(sprites);

  instance_renderer->SetCamera({0.3, 0.3}, 0.5);
}

void Application::CreateSyncObjects() {
  VkSemaphoreCreateInfo semphore_create_info =
      vk::semaphore_create_info_template;

  VkFenceCreateInfo fence_create_info = vk::fence_create_info_template;

  VkResult result;

  result = vkCreateSemaphore(device->GetHandle(), &semphore_create_info,
                             nullptr, &image_available_semaphore);
  if (result) {
    throw vk::CriticalException("cant create image available semaphore");
  }

  result = vkCreateSemaphore(device->GetHandle(), &semphore_create_info,
                             nullptr, &render_finished_semaphore);
  if (result) {
    throw vk::CriticalException("cant create render fished semaphore");
  }

  result =
      vkCreateFence(device->GetHandle(), &fence_create_info, nullptr, &fence);
  if (result) {
    throw vk::CriticalException("cant create fence");
  }

  TRACE("image available semaphore created");
  TRACE("render finished semaphore created");
  TRACE("fence created");
}

void Application::CleanupSyncObjects() {
  vkDestroySemaphore(device->GetHandle(), image_available_semaphore, nullptr);
  vkDestroySemaphore(device->GetHandle(), render_finished_semaphore, nullptr);
  vkDestroyFence(device->GetHandle(), fence, nullptr);
}

void Application::RenderLoop() {
  DEBUG("render loop launched");

  while (!window->ShouldClose()) {
    Update();

    try {
      Draw();
    } catch (vk::AcquireNextImageFailedException e) {
      ChangeSurface();
    } catch (vk::PresentFailedException e) {
      ChangeSurface();
    }

    Window::PollEvents();
  }

  DEBUG("render loop exit");
}

void Application::ChangeSurface() {
  vkDeviceWaitIdle(device->GetHandle());

  swapchain.reset();

  window->DestroySurface();
  window->CreateSurface();

  swapchain = make_unique<vk::Swapchain>(*device, window->GetSurface());

  CleanupSyncObjects();
  CreateSyncObjects();

  CleanupFramebuffers();
  CreateFramebuffers();

  CreateInstanceRenderer();
}

void Application::PreUpdate() { time_from_start = now() - program_start; }

void Application::Update() {
  return;
  PreUpdate();
  UpdateRenderData();
}

void Application::UpdateRenderData() {}

void Application::Draw() {
  uint32_t next_image_index =
      swapchain->AcquireNextImage(image_available_semaphore);

  Render(next_image_index);

  Present(next_image_index);

  vkWaitForFences(device->GetHandle(), 1, &fence, VK_TRUE, UINT64_MAX);
  vkResetFences(device->GetHandle(), 1, &fence);
}

void Application::Render(uint32_t next_image_index) {
  instance_renderer->Render(next_image_index, image_available_semaphore,
                            render_finished_semaphore, fence);
}

void Application::Present(uint32_t next_image_index) {
  VkSwapchainKHR swapchain_handle = swapchain->GetHandle();

  VkPresentInfoKHR present_info = vk::present_info_template;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &render_finished_semaphore;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swapchain_handle;
  present_info.pImageIndices = &next_image_index;

  VkResult result =
      vkQueuePresentKHR(graphics_queue.GetHandle(), &present_info);
  if (result) {
    throw vk::PresentFailedException();
  }
}

void Application::CreateFramebuffers() {
  VkFramebufferCreateInfo create_info = vk::framebuffer_create_info_template;
  create_info.renderPass = render_pass;
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

void Application::CleanupFramebuffers() {
  for (int i = 0; i < framebuffers.size(); i++) {
    vkDestroyFramebuffer(device->GetHandle(), framebuffers[i], nullptr);
  }
}

void Application::CreateRenderPass() {
  VkAttachmentDescription color_attachment =
      vk::attachment_description_template;
  color_attachment.format = swapchain->GetFormat().format;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
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
                                       nullptr, &render_pass);
  if (result) {
    throw vk::CriticalException("cant create render pass");
  }

  DEBUG("render pass created");
}

void Application::CreateInstance() {
  vk::InstanceCreateInfo create_info;

  create_info.layers.push_back("VK_LAYER_KHRONOS_validation");
  create_info.extensions.push_back("VK_EXT_debug_utils");

  uint32_t glfw_extensions_count;
  const char **glfw_extensions;
  window->GetInstanceExtensions(glfw_extensions, glfw_extensions_count);

  for (int i = 0; i < glfw_extensions_count; i++) {
    create_info.extensions.push_back(glfw_extensions[i]);
  }

  instance = unique_ptr<vk::Instance>(new vk::Instance(create_info));
}

void Application::CreateDevice() {
  shared_ptr<vk::PhysicalDevice> physical_device =
      instance->physical_devices[0];

  vk::DeviceCreateInfo::QueueRequest graphics_queue_request;
  graphics_queue_request.flags = VK_QUEUE_GRAPHICS_BIT;
  graphics_queue_request.queue = &graphics_queue;

  vk::DeviceCreateInfo create_info;
  create_info.queue_requests.push_back(graphics_queue_request);

  device = unique_ptr<vk::Device>(new vk::Device(physical_device, create_info));
}
