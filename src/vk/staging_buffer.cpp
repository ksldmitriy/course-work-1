#include "staging_buffer.hpp"

namespace vk {

StagingBuffer::StagingBuffer(Device &device,
                             StagingBufferCreateInfo &create_info) {
  this->device = &device;
  this->queue = queue;
  this->command_buffer = create_info.command_buffer;

  CreateBuffer(create_info.size);
  CreateMemory();

  memory->BindBuffer(*buffer);
}

void StagingBuffer::CreateBuffer(VkDeviceSize size) {
  BufferCreateInfo buffer_create_info;
  buffer_create_info.queue = queue;
  buffer_create_info.size = size;
  buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  buffer = make_unique<Buffer>(*device, buffer_create_info);
}

void StagingBuffer::CreateMemory() {
  vector<MemoryObject *> memory_objects = {buffer.get()};
  VkDeviceSize memory_size = DeviceMemory::CalculateMemorySize(memory_objects);

  PhysicalDevice &physical_device = device->GetPhysicalDevice();

  vk::ChooseMemoryTypeInfo choose_info;
  choose_info.memory_types = buffer->GetMemoryTypes();
  choose_info.heap_properties = 0;
  choose_info.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

  uint32_t memory_type = physical_device.ChooseMemoryType(choose_info);

  memory = make_unique<vk::DeviceMemory>(*device, memory_size, memory_type);
}

void StagingBuffer::CopyToBuffer(Buffer *dst_buffer, VkDeviceSize size,
                                 VkDeviceSize src_offet,
                                 VkDeviceSize dst_offet) {
  if (size = VK_WHOLE_SIZE) {
    size = dst_buffer->GetSize();
  }

  VkBufferCopy region;
  region.size = size;
  region.dstOffset = src_offet;
  region.srcOffset = dst_offet;

  vkCmdCopyBuffer(command_buffer->GetHandle(), buffer->GetHandle(),
                  dst_buffer->GetHandle(), 1, &region);
}

MemoryBarrier StagingBuffer::CreateLoadDataBarrier() {
  DstMemoryBarrier dst;
  dst.stage = VK_PIPELINE_STAGE_HOST_BIT;
  dst.access = VK_ACCESS_HOST_WRITE_BIT;

  SrcMemoryBarrier src;
  src.stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  src.access = VK_ACCESS_TRANSFER_READ_BIT;

  MemoryBarrier barrier(*buffer, src, dst);

  return barrier;
}

SrcImageBarrier StagingBuffer::CopyToImage(Image *image,
                                           SrcImageBarrier &src_barrier) {
  SetPreloadImageBarrier(image);

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = image->GetExtent();

  vkCmdCopyBufferToImage(command_buffer->GetHandle(), buffer->GetHandle(),
                         image->GetHandle(),
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  TRACE("data copy instruction from staging buffer to image wrote to command buffer");
  
  return CreateAfterloadImageBarrier(image);
}

void StagingBuffer::SetPreloadImageBarrier(Image *image) {
  VkImageLayout old_layout =
      image->ChangeLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  SrcImageBarrier src_barrier;
  src_barrier.layout = old_layout;
  src_barrier.access = 0;
  src_barrier.stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

  DstImageBarrier dst_barrier;
  dst_barrier.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  dst_barrier.access = VK_ACCESS_TRANSFER_WRITE_BIT;
  dst_barrier.stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

  ImageBarrier barrier(*image, src_barrier, dst_barrier);

  barrier.Set(*command_buffer);
}

SrcImageBarrier StagingBuffer::CreateAfterloadImageBarrier(Image *image) {
  SrcImageBarrier barrier;
  barrier.layout = image->GetLayout();
  barrier.access = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

  return barrier;
}

StagingBuffer::~StagingBuffer() {
  if (memory) {
    Free();
  }
}

void StagingBuffer::Free() {
  buffer->Destroy();
  memory->Free();
}

void StagingBuffer::LoadData(span<char> data) {
  char *mapped_memory = (char *)buffer->Map();

  memcpy(mapped_memory, data.data(), data.size_bytes());

  buffer->Flush();
  buffer->Unmap();

  MemoryBarrier barrier = CreateLoadDataBarrier();
  barrier.Set(*command_buffer);
}

} // namespace vk
