#include "image_view.hpp"

namespace vk {

ImageView::ImageView(Device *device, Image *image) {
  this->device = device;

  VkImageSubresourceRange subresource_range = image_subresource_range_template;
  subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  VkImageViewCreateInfo create_info = image_view_create_info_template;
  create_info.image = image->GetHandle();
  create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  create_info.format = image->GetFormat();
  create_info.subresourceRange = subresource_range;

  VkResult result =
      vkCreateImageView(device->GetHandle(), &create_info, nullptr, &handle);
  if (result) {
    throw CriticalException("cant create image view");
  }

  TRACE("image view created");
}

ImageView::~ImageView() { Destroy(); }

void ImageView::Destroy() {
  if (handle == VK_NULL_HANDLE) {
    return;
  }

  vkDestroyImageView(device->GetHandle(), handle, nullptr);
  handle = VK_NULL_HANDLE;

  TRACE("image view destroyed");
}

VkImageView ImageView::GetHandle() { return handle; }

} // namespace vk
