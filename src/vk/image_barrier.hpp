#pragma once
#include "command_buffer.hpp"
#include "templates.hpp"
#include <vulkan/vulkan.h>

using namespace std;

namespace vk {

class Image;

struct SrcImageBarrier {
  VkPipelineStageFlags stage;
  VkAccessFlags access;
  uint32_t queue_family_index = VK_QUEUE_FAMILY_IGNORED;
  VkImageLayout layout;
};

struct DstImageBarrier {
  VkPipelineStageFlags stage;
  VkAccessFlags access;
  uint32_t queue_family_index = VK_QUEUE_FAMILY_IGNORED;
  VkImageLayout layout;
};

class ImageBarrier {
private:
  SrcImageBarrier src;
  DstImageBarrier dst;

  VkImage image;

  VkImageMemoryBarrier ToNative();

public:
  ImageBarrier(Image &image, SrcImageBarrier src, DstImageBarrier dst);

  void Set(CommandBuffer &command_buffer);
};

} // namespace vk
