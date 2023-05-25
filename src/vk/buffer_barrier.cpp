#include "buffer_barrier.hpp"

namespace vk {

BufferBarrier::BufferBarrier(Buffer *buffer, SrcBufferBarrier src,
                             DstBufferBarrier dst) {
  this->src = src;
  this->dst = dst;
  this->buffer = buffer;
}

void BufferBarrier::Set(CommandBuffer *command_buffer) {
  VkBufferMemoryBarrier barrier = ToNative();

  vkCmdPipelineBarrier(command_buffer->GetHandle(), src.stage, dst.stage, 0, 0,
                       nullptr, 1, &barrier, 0, nullptr);
}

VkBufferMemoryBarrier BufferBarrier::ToNative() {
  VkBufferMemoryBarrier barrier = buffer_barrier_template;
  barrier.buffer = buffer->GetHandle();
  barrier.offset = 0;
  barrier.size = VK_WHOLE_SIZE;

  barrier.srcAccessMask = src.access;
  barrier.srcQueueFamilyIndex = src.queue_family;

  barrier.dstAccessMask = dst.access;
  barrier.dstQueueFamilyIndex = dst.queue_family;

  return barrier;
}

} // namespace vk
