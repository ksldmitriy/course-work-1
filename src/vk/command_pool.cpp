#include "command_pool.hpp"

namespace vk {

CommandPool::CommandPool(Device &device, Queue &queue, uint32_t capacity) {
  this->device = &device;
  this->capacity = capacity;
  size = 0;

  int count = 3;

  VkCommandPoolCreateInfo create_info = command_pool_create_info_template;
  create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  create_info.queueFamilyIndex = queue.GetFamily();

  VkResult result =
      vkCreateCommandPool(device.GetHandle(), &create_info, nullptr, &handle);
  if (result) {
    throw CriticalException("cant create command buffer pool");
  }

  TRACE("command pool with {0} elements capacity created", capacity);
}

CommandPool::~CommandPool() {
  if (handle == VK_NULL_HANDLE) {
    return;
  }

  Dispose();
};

void CommandPool::Dispose() {
  vkDestroyCommandPool(device->GetHandle(), handle, nullptr);
  handle = VK_NULL_HANDLE;

  TRACE("command pool destroyed");
}

unique_ptr<CommandBuffer>
CommandPool::AllocateCommandBuffer(CommandBufferLevel level) {
  if (size >= capacity) {
    throw CriticalException(
        "no space in command pool to allocate command buffer");
  }

  size++;

  TRACE("command buffer allocated, pool capacity is {0}/{1}", size, capacity);

  return unique_ptr<CommandBuffer>(new CommandBuffer(*this, level));
};

void CommandPool::DisposeCommandBufferCallback() {
  size--;
  TRACE("command buffer freed, pool capacity is {0}/{1}", size, capacity);
}

VkCommandPool CommandPool::GetHandle() { return handle; }

} // namespace vk
