#include "image_barrier.hpp"
#include "image.hpp"

namespace vk {

ImageBarrier::ImageBarrier(Image &image, SrcImageBarrier src,
                           DstImageBarrier dst) {
  this->src = src;
  this->dst = dst;

  this->image = image.GetHandle();
}

void ImageBarrier::Set(CommandBuffer &command_buffer) {
  VkImageMemoryBarrier image_barrier = ToNative();

  vkCmdPipelineBarrier(command_buffer.GetHandle(), src.stage, dst.stage, 0, 0, nullptr, 0,
                       nullptr, 1, &image_barrier);
}

VkImageMemoryBarrier ImageBarrier::ToNative() {
  VkImageMemoryBarrier image_barrier = image_memory_barrier_template;

  image_barrier.image = image;

  image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_barrier.subresourceRange.baseMipLevel = 0;
  image_barrier.subresourceRange.levelCount = 1;
  image_barrier.subresourceRange.baseArrayLayer = 0;
  image_barrier.subresourceRange.layerCount = 1;

  image_barrier.srcAccessMask = src.access;
  image_barrier.srcQueueFamilyIndex = src.queue_family_index;
  image_barrier.oldLayout = src.layout;

  image_barrier.dstAccessMask = dst.access;
  image_barrier.dstQueueFamilyIndex = dst.queue_family_index;
  image_barrier.newLayout = dst.layout;

  return image_barrier;
}

} // namespace vk
