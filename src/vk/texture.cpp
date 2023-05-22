#include "texture.hpp"

namespace vk {

Texture::Texture(Device *device) { this->device = device; }

Texture::~Texture() { Destroy(); }

void Texture::Destroy() {
  if (image) {
    image->Destroy();
    image.reset();
  }

  if(memory){
	memory->Free();
	memory.reset();
  }
  
  TRACE("texture destoyed");
}

void Texture::LoadImage(char *image_data, glm::ivec2 image_size,
                        CommandBuffer *command_buffer, Queue graphics_queue) {
  vk::ImageCreateInfo image_crate_info;
  image_crate_info.size = image_size;
  image_crate_info.format = VK_FORMAT_R8G8B8A8_SRGB;

  image = make_unique<vk::Image>(*device, image_crate_info);

  vector<vk::MemoryObject *> memory_objects = {image.get()};
  VkDeviceSize memory_size =
      vk::DeviceMemory::CalculateMemorySize(memory_objects);

  vk::PhysicalDevice &physical_device = device->GetPhysicalDevice();

  vk::ChooseMemoryTypeInfo choose_info;
  choose_info.memory_types = image->GetMemoryTypes();
  choose_info.heap_properties = 0;
  choose_info.properties = 0;

  uint32_t memory_type = physical_device.ChooseMemoryType(choose_info);

  memory = make_unique<vk::DeviceMemory>(*device, memory_size, memory_type);

  memory->BindImage(*image);

  // load data to image
  int image_size_in_bytes = image_size.x * image_size.y * 4;

  vk::StagingBufferCreateInfo staging_buffer_create_info;
  staging_buffer_create_info.command_buffer = command_buffer;
  staging_buffer_create_info.queue = graphics_queue;
  staging_buffer_create_info.size = image_size_in_bytes;

  unique_ptr<vk::StagingBuffer> staging_buffer =
      make_unique<vk::StagingBuffer>(*device, staging_buffer_create_info);

  command_buffer->Begin();

  staging_buffer->LoadData(span<char>(image_data, image_size_in_bytes));

  vk::SrcImageBarrier src_barrier;

  vk::SrcImageBarrier afterload_src_barrier =
      staging_buffer->CopyToImage(image.get(), src_barrier);

  image->ChangeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vk::DstImageBarrier afterload_dst_barrier;
  afterload_dst_barrier.access = VK_ACCESS_SHADER_READ_BIT;
  afterload_dst_barrier.layout = image->GetLayout();
  afterload_dst_barrier.stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

  vk::ImageBarrier afterload_barrier(*image, afterload_src_barrier,
                                     afterload_dst_barrier);
  afterload_barrier.Set(*command_buffer);

  command_buffer->End();

  command_buffer->SoloExecute();

  TRACE("image loaded to texture");
}

unique_ptr<ImageView> Texture::CreateImageView() {
  unique_ptr<ImageView> image_view =
      make_unique<ImageView>(device, image.get());

  return image_view;
}

} // namespace vk
