#pragma once
#include "exception.hpp"
#include <vector>
#include <vulkan/vulkan.h>

using namespace std;

namespace vk {

struct ChooseMemoryTypeInfo {
  VkMemoryPropertyFlags properties;
  VkMemoryHeapFlags heap_properties;
  uint32_t memory_types;
};

class PhysicalDevice {
private:
  VkPhysicalDevice handle;
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceMemoryProperties memory_properties;
  vector<VkQueueFamilyProperties> queue_families_properties;

public:
  PhysicalDevice(VkPhysicalDevice handle);
  PhysicalDevice(PhysicalDevice &) = delete;
  PhysicalDevice &operator=(PhysicalDevice &) = delete;

  VkPhysicalDevice GetHandle();
  VkPhysicalDeviceLimits GetLimits();
  uint32_t ChooseQueueFamily(VkQueueFlags requirements);
  uint32_t ChooseMemoryType(ChooseMemoryTypeInfo &choose_info);
  VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkSurfaceKHR surface);
  vector<VkSurfaceFormatKHR> GetSurfaceFormats(VkSurfaceKHR surface);
  vector<VkPresentModeKHR> GetSurfacePresentModes(VkSurfaceKHR surface);
};

} // namespace vk
