#include "image.hpp"
#include "device_memory.hpp"

namespace vk {

Image::Image(Device &device, ImageCreateInfo &create_info) {
  this->device = &device;

  VkImageCreateInfo vk_create_info = image_create_info_template;
  vk_create_info.imageType = VK_IMAGE_TYPE_2D;
  vk_create_info.extent.width = create_info.size.x;
  vk_create_info.extent.height = create_info.size.y;
  vk_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
  vk_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  vk_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
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

void Image::Destroy() {
  vkDestroyImage(device->GetHandle(), handle, nullptr);

  handle = VK_NULL_HANDLE;

  TRACE("image destroyed");
}

VkImage Image::GetHandle() { return handle; }

} // namespace vk
