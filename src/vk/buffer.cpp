#include "buffer.hpp"
#include "device_memory.hpp"

namespace vk {

Buffer::Buffer(Device &device, BufferCreateInfo &create_info) {
  this->device = &device;
  is_binded = false;
  size = create_info.size;
  
  VkBufferCreateInfo vk_create_info;
  vk_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  vk_create_info.pNext = nullptr;
  vk_create_info.flags = 0;

  uint32_t queue_family = create_info.queue.GetFamily();

  vk_create_info.size = create_info.size;
  vk_create_info.usage = create_info.usage;
  vk_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  vk_create_info.queueFamilyIndexCount = 1;
  vk_create_info.pQueueFamilyIndices = &queue_family;

  VkResult result =
      vkCreateBuffer(device.GetHandle(), &vk_create_info, nullptr, &handle);
  if (result) {
    throw CriticalException("cant create buffer");
  }

  vkGetBufferMemoryRequirements(device.GetHandle(), handle, &requirements);
}

Buffer::~Buffer() {
  if (handle && is_binded) {
    Destroy();
  }
}

void Buffer::Destroy() {
  memory->FreeBuffer(this);

  is_binded = false;
  vkDestroyBuffer(device->GetHandle(), handle, nullptr);
  handle = 0;
}

void *Buffer::Map() { return memory->MapMemory(this); }

void Buffer::Flush() { memory->Flush(); }

void Buffer::Unmap() { memory->Unmap(); }

VkBuffer Buffer::GetHandle() { return handle; }

VkMemoryRequirements Buffer::GetMemoryRequirements() { return requirements; }

uint32_t Buffer::GetMemoryTypes() { return requirements.memoryTypeBits; }

VkDeviceSize Buffer::GetSize() { return size; }

} // namespace vk
