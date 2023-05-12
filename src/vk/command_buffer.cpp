#include "command_buffer.hpp"
#include "command_pool.hpp"
#include "exception.hpp"
#include "templates.hpp"
#include <vulkan/vulkan_core.h>

namespace vk {

CommandBuffer::CommandBuffer(CommandPool &pool, CommandBufferLevel level) {
  this->pool = &pool;

  VkCommandBufferAllocateInfo allocate_info =
      command_buffer_allocate_info_template;
  allocate_info.level = (VkCommandBufferLevel)level;
  allocate_info.commandPool = pool.GetHandle();
  allocate_info.commandBufferCount = 1;

  VkResult result = vkAllocateCommandBuffers(pool.device->GetHandle(),
                                             &allocate_info, &handle);
  if (result) {
    throw CriticalException("cant allocate command buffer");
  }
}

CommandBuffer::~CommandBuffer() {
  if (handle == VK_NULL_HANDLE) {
    return;
  }

  Dispose();
}

VkCommandBuffer CommandBuffer::GetHandle() { return handle; }

void CommandBuffer::Begin() {
  VkCommandBufferBeginInfo begin_info = vk::command_buffer_begin_info_template;

  VkResult result = vkBeginCommandBuffer(handle, &begin_info);
  if (result) {
    throw CriticalException("cant begin command buffer");
  }

  TRACE("command buffer begun");
}

void CommandBuffer::End() {
  VkResult result = vkEndCommandBuffer(handle);
  if (result) {
    throw CriticalException("cant end command buffer");
  }

  TRACE("command buffer ended");
}

void CommandBuffer::Dispose() {
  vkFreeCommandBuffers(pool->device->GetHandle(), pool->GetHandle(), 1,
                       &handle);

  pool->DisposeCommandBufferCallback();

  handle = VK_NULL_HANDLE;
}

} // namespace vk
