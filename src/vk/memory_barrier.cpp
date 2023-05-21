#include "memory_barrier.hpp"

namespace vk {

MemoryBarrier::MemoryBarrier(Buffer &buffer, SrcMemoryBarrier src,
                             DstMemoryBarrier dst) {
  this->src = src;
  this->dst = dst;
  this->buffer = buffer.GetHandle();
}

void MemoryBarrier::Set(CommandBuffer &command_buffer) {
  VkMemoryBarrier memory_barrier = ToNative();

  vkCmdPipelineBarrier(command_buffer.GetHandle(), src.stage, dst.stage, 0, 1,
                       &memory_barrier, 0, nullptr, 0, nullptr);
}

VkMemoryBarrier MemoryBarrier::ToNative() {
  VkMemoryBarrier memory_barrier = memory_barrier_template;
  memory_barrier.srcAccessMask = src.access;
  memory_barrier.dstAccessMask = dst.access;

  return memory_barrier;
}

} // namespace vk
