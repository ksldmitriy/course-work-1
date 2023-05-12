#include "swapchain.hpp"
#include "../logs.hpp"
#include "templates.hpp"

namespace vk {

Swapchain::Swapchain(Device &device, VkSurfaceKHR surface) {
  this->device = &device;

  vk::PhysicalDevice &physical_device = device.GetPhysicalDevice();

  // get capabilities
  VkSurfaceCapabilitiesKHR surface_capabilities =
      physical_device.GetSurfaceCapabilities(surface);
  vector<VkSurfaceFormatKHR> supported_formats =
      physical_device.GetSurfaceFormats(surface);

  // choose swapchain details
  ChooseSurfaceFormat(supported_formats);

  uint32_t desired_images_count =
      ChooseSwapchainImagesCount(surface_capabilities);

  extent = ChooseSwapchainExtend(surface_capabilities);

  // create swapchain
  VkSwapchainCreateInfoKHR create_info;
  GenerateCreateInfo(physical_device, create_info, surface,
                     desired_images_count, surface_capabilities);

  VkResult result =
      vkCreateSwapchainKHR(device.GetHandle(), &create_info, nullptr, &handle);
  if (result) {
    throw CriticalException("cant create swapchain");
  }

  DEBUG("swapchain created");

  // get images
  GetImagesFromDevice(device);

  CreateImageViews(device);
}

Swapchain::~Swapchain() {
  if (handle == VK_NULL_HANDLE) {
    return;
  }

  Dispose();
}

void Swapchain::Dispose() {
  for (int i = 0; i < images.size(); i++) {
    vkDestroyImageView(device->GetHandle(), image_views[i], nullptr);
  }

  vkDestroySwapchainKHR(device->GetHandle(), handle, nullptr);
  handle = VK_NULL_HANDLE;

  DEBUG("swapchain disposed");
}

VkSwapchainKHR Swapchain::GetHandle() { return handle; }

uint32_t Swapchain::AcquireNextImage(VkSemaphore semaphore) {
  uint32_t next_image;

  VkResult result =
      vkAcquireNextImageKHR(device->GetHandle(), handle, UINT64_MAX, semaphore,
                            VK_NULL_HANDLE, &next_image);
  if (result) {
    throw AcquireNextImageFailedException();
  }

  return next_image;
}

const vector<VkImage> &Swapchain::GetImages() { return images; }

const vector<VkImageView> &Swapchain::GetImageViews() { return image_views; }

VkSurfaceFormatKHR Swapchain::GetFormat() { return format; }

VkExtent2D Swapchain::GetExtent() { return extent; }

void Swapchain::CreateImageViews(Device &device) {
  image_views.resize(images.size());

  VkImageViewCreateInfo create_info = image_view_create_info_template;
  create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  create_info.format = format.format;
  create_info.components.r = create_info.components.g =
      create_info.components.b = create_info.components.a =
          VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  create_info.subresourceRange.baseMipLevel = 0;
  create_info.subresourceRange.levelCount = 1;
  create_info.subresourceRange.baseArrayLayer = 0;
  create_info.subresourceRange.layerCount = 1;

  for (int i = 0; i < image_views.size(); i++) {
    create_info.image = images[i];
    VkResult result = vkCreateImageView(device.GetHandle(), &create_info,
                                        nullptr, &image_views[i]);
  }

  TRACE("swapchain image views created");
}

void Swapchain::GenerateCreateInfo(PhysicalDevice &physical_device,
                                   VkSwapchainCreateInfoKHR &create_info,
                                   VkSurfaceKHR surface,
                                   uint32_t desired_images_count,
                                   VkSurfaceCapabilitiesKHR capabilities) {

  vector<VkPresentModeKHR> supported_present_modes =
      physical_device.GetSurfacePresentModes(surface);

  VkPresentModeKHR present_mode = ChoosePresentMode(supported_present_modes);

  VkSurfaceTransformFlagBitsKHR surface_transform =
      ChooseSurfaceTransform(capabilities);

  create_info = vk::swapchain_create_info_template;
  create_info.surface = surface;
  create_info.minImageCount = desired_images_count;
  create_info.imageFormat = format.format;
  create_info.imageColorSpace = format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  create_info.preTransform = surface_transform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = nullptr;
}

void Swapchain::GetImagesFromDevice(Device &device) {
  VkResult result;

  uint32_t images_count = 0;
  result = vkGetSwapchainImagesKHR(device.GetHandle(), handle, &images_count,
                                   nullptr);
  if (result) {
    throw CriticalException("cant get swapchain images count");
  } else if (!images_count) {
    throw CriticalException("swapchain images count is 0");
  }

  TRACE("swapchain images count is {}", images_count);

  images.resize(images_count);
  result = vkGetSwapchainImagesKHR(device.GetHandle(), handle, &images_count,
                                   images.data());
  if (result) {
    throw CriticalException("cant get swapchain images");
  }
}

uint32_t
Swapchain::ChooseSwapchainImagesCount(VkSurfaceCapabilitiesKHR &capabilities) {
  uint32_t desired_swapchain_images_count = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount &&
      desired_swapchain_images_count > capabilities.maxImageCount) {
    desired_swapchain_images_count = capabilities.maxImageCount;
  }

  return desired_swapchain_images_count;
}

VkExtent2D Swapchain::ChooseSwapchainExtend(
    VkSurfaceCapabilitiesKHR surface_capabilities) {
  return surface_capabilities.currentExtent;
}

VkSurfaceTransformFlagBitsKHR Swapchain::ChooseSurfaceTransform(
    VkSurfaceCapabilitiesKHR surface_capabilities) {
  return surface_capabilities.currentTransform;
}

VkPresentModeKHR Swapchain::ChoosePresentMode(
    vector<VkPresentModeKHR> &supported_present_modes) {
  if (find(supported_present_modes.begin(), supported_present_modes.end(),
           VK_PRESENT_MODE_FIFO_KHR) != supported_present_modes.end()) {
    TRACE("present mode is fifo");
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  TRACE("present mode is not fifo");
  return supported_present_modes[0];
}

void Swapchain::ChooseSurfaceFormat(
    vector<VkSurfaceFormatKHR> &supported_formats) {
  if (supported_formats.size() == 1 &&
      supported_formats[0].format == VK_FORMAT_UNDEFINED) {
    format = {VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
    return;
  }

  for (const auto &format : supported_formats) {
    if (format.format == VK_FORMAT_R8G8B8A8_UNORM) {
      this->format = format;
      return;
    }
  }

  format = supported_formats[0];
}

} // namespace vk
