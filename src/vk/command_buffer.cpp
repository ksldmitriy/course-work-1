#include "command_buffer.hpp"
#include "command_pool.hpp"
#include "exception.hpp"
#include "templates.hpp"
#include <vulkan/vulkan_core.h>

namespace vk {

CommandBuffer::CommandBuffer(CommandBufferCreateInfo &create_info) {
  this->pool = create_info.pool;
  this->queue = create_info.queue;

  VkCommandBufferAllocateInfo allocate_info =
      command_buffer_allocate_info_template;
  allocate_info.level = (VkCommandBufferLevel)create_info.level;
  allocate_info.commandPool = pool->GetHandle();
  allocate_info.commandBufferCount = 1;

  VkResult result = vkAllocateCommandBuffers(pool->device->GetHandle(),
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

void CommandBuffer::SoloExecute() {
  VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

  VkSubmitInfo submit_info = vk::submit_info_template;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &handle;

  VkResult result =
      vkQueueSubmit(queue.GetHandle(), 1, &submit_info, pool->solo_execute_fence);
  if (result) {
    throw vk::CriticalException("cant submit to queue");
  }

  vkWaitForFences(pool->device->GetHandle(), 1, &pool->solo_execute_fence, VK_TRUE,
                  UINT64_MAX);

  vkResetFences(pool->device->GetHandle(), 1, &pool->solo_execute_fence);
  
  TRACE("command pool solo executed");
}

void CommandBuffer::Reset() { vkResetCommandBuffer(handle, 0); }

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
