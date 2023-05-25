#pragma once
#include "device.hpp"
#include "exception.hpp"
#include "instance.hpp"
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

using namespace std;

namespace vk {

class Swapchain {
private:
  VkSwapchainKHR handle;

  Device *device;

  VkSurfaceFormatKHR format;
  VkExtent2D extent;
  VkPresentModeKHR present_mode;

  uint32_t min_image_count;

  vector<VkImage> images;
  vector<VkImageView> image_views;

  void GenerateCreateInfo(PhysicalDevice &physical_device,
                          VkSwapchainCreateInfoKHR &create_info,
                          VkSurfaceKHR surface, uint32_t desired_images_count,
                          VkSurfaceCapabilitiesKHR capabilities);
  void GetImagesFromDevice(Device &device);
  void CreateImageViews(Device &device);

  uint32_t ChooseSwapchainImagesCount(VkSurfaceCapabilitiesKHR &capabilities);
  void ChooseSurfaceFormat(vector<VkSurfaceFormatKHR> &supported_formats);
  VkPresentModeKHR
  ChoosePresentMode(vector<VkPresentModeKHR> &supported_present_modes,
                    VkPresentModeKHR prefered_present_mode);
  VkSurfaceTransformFlagBitsKHR
  ChooseSurfaceTransform(VkSurfaceCapabilitiesKHR surface_capabilities);
  VkExtent2D
  ChooseSwapchainExtend(VkSurfaceCapabilitiesKHR surface_capabilities);

public:
  Swapchain(Device &device, VkSurfaceKHR surface);
  ~Swapchain();
  Swapchain(Swapchain &) = delete;
  Swapchain &operator=(Swapchain &) = delete;

  void Dispose();

  VkSwapchainKHR GetHandle();
  uint32_t GetMinImageCount();
  uint32_t AcquireNextImage(VkSemaphore semaphore);
  VkSurfaceFormatKHR GetFormat();
  VkExtent2D GetExtent();
  VkPresentModeKHR GetPresentMode();
  const vector<VkImage> &GetImages();
  const vector<VkImageView> &GetImageViews();
};

} // namespace vk
