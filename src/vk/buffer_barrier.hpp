#pragma once
#include "buffer.hpp"
#include "command_buffer.hpp"

using namespace std;

namespace vk {

struct SrcBufferBarrier {
  VkPipelineStageFlags stage;
  VkAccessFlags access;
  uint32_t queue_family = VK_QUEUE_FAMILY_IGNORED;
};

struct DstBufferBarrier {
  VkPipelineStageFlags stage;
  VkAccessFlags access;
  uint32_t queue_family = VK_QUEUE_FAMILY_IGNORED;
};

class BufferBarrier {
private:
  SrcBufferBarrier src;
  DstBufferBarrier dst;

  Buffer *buffer;

  VkBufferMemoryBarrier ToNative();

public:
  BufferBarrier(Buffer *buffer, SrcBufferBarrier src, DstBufferBarrier dst);

  void Set(CommandBuffer *command_buffer);
};

} // namespace vk
