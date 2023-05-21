#include "image.hpp"
#include "device_memory.hpp"

namespace vk {

Image::Image(Device &device, ImageCreateInfo &create_info) {
  this->device = &device;
  format = create_info.format;
  current_layout = create_info.layout;

  extent.width = create_info.size.x;
  extent.height = create_info.size.y;
  extent.depth = 1;

  VkImageCreateInfo vk_create_info = image_create_info_template;
  vk_create_info.imageType = VK_IMAGE_TYPE_2D;
  vk_create_info.extent = extent;
  vk_create_info.format = format;
  vk_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  vk_create_info.initialLayout = current_layout;
  vk_create_info.usage =
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkResult result =
      vkCreateImage(device.GetHandle(), &vk_create_info, nullptr, &handle);
  if (result) {
    throw CriticalException("cant create image");
  }

  vkGetImageMemoryRequirements(device.GetHandle(), handle, &requirements);

  TRACE("image created");
}

Image::~Image() {
  if (handle) {
    Destroy();
  }
}

VkImageLayout Image::GetLayout() { return current_layout; }

VkImageLayout Image::ChangeLayout(VkImageLayout new_layout) {
  VkImageLayout old_layout = current_layout;
  current_layout = new_layout;

  return old_layout;
}

VkExtent3D Image::GetExtent() { return extent; }

VkFormat Image::GetFormat() { return format; }

void Image::Destroy() {
  vkDestroyImage(device->GetHandle(), handle, nullptr);

  handle = VK_NULL_HANDLE;

  TRACE("image destroyed");
}

VkImage Image::GetHandle() { return handle; }

} // namespace vk
