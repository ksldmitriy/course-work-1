#pragma once
#include "command_buffer.hpp"
#include "templates.hpp"
#include <vulkan/vulkan.h>
#include "buffer.hpp"

using namespace std;

namespace vk {

struct SrcMemoryBarrier {
  VkPipelineStageFlags stage;
  VkAccessFlags access;
};

struct DstMemoryBarrier {
  VkPipelineStageFlags stage;
  VkAccessFlags access;
};

class MemoryBarrier {
private:
  SrcMemoryBarrier src;
  DstMemoryBarrier dst;

  VkBuffer buffer;
  
  VkMemoryBarrier ToNative();

public:
  MemoryBarrier(Buffer& buffer, SrcMemoryBarrier src, DstMemoryBarrier dst);

  void Set(CommandBuffer &command_buffer);
};

} // namespace vk
