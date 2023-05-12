#include "physical_device.hpp"
#include "exception.hpp"

namespace vk {

PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle) {
  this->handle = handle;

  vkGetPhysicalDeviceProperties(handle, &properties);

  vkGetPhysicalDeviceMemoryProperties(handle, &memory_properties);

  uint32_t queue_families_count;
  vkGetPhysicalDeviceQueueFamilyProperties(handle, &queue_families_count,
                                           nullptr);
  queue_families_properties.resize(queue_families_count);
  vkGetPhysicalDeviceQueueFamilyProperties(handle, &queue_families_count,
                                           queue_families_properties.data());
}

VkSurfaceCapabilitiesKHR
PhysicalDevice::GetSurfaceCapabilities(VkSurfaceKHR surface) {
  VkSurfaceCapabilitiesKHR surface_capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface,
                                            &surface_capabilities);

  return surface_capabilities;
}

vector<VkSurfaceFormatKHR>
PhysicalDevice::GetSurfaceFormats(VkSurfaceKHR surface) {
  uint32_t formats_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formats_count,
                                       nullptr);

  std::vector<VkSurfaceFormatKHR> surface_formats(formats_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formats_count,
                                       surface_formats.data());

  return surface_formats;
}

vector<VkPresentModeKHR>
PhysicalDevice::GetSurfacePresentModes(VkSurfaceKHR surface) {
  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface,
                                            &present_mode_count, nullptr);

  std::vector<VkPresentModeKHR> present_modes(present_mode_count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      handle, surface, &present_mode_count, present_modes.data());

  return present_modes;
}

VkPhysicalDevice PhysicalDevice::GetHandle() { return handle; }

VkPhysicalDeviceLimits PhysicalDevice::GetLimits() { return properties.limits; }

uint32_t PhysicalDevice::ChooseQueueFamily(VkQueueFlags requirements) {
  for (uint32_t i = 0; i < queue_families_properties.size(); i++) {
    if ((queue_families_properties[i].queueFlags & requirements) ==
        requirements) {
      return i;
    }
  }

  throw QueueFamilyNotFoundException();
};

uint32_t PhysicalDevice::ChooseMemoryType(ChooseMemoryTypeInfo &choose_info) {
  bool valid_heaps[VK_MAX_MEMORY_HEAPS] = {0};

  for (int i = 0; i < memory_properties.memoryHeapCount; i++) {
    VkMemoryHeap heap = memory_properties.memoryHeaps[i];
    if ((heap.flags & choose_info.heap_properties) ==choose_info.heap_properties) {
      valid_heaps[i] = true;
    };
  }

  for (int i = 0; i < memory_properties.memoryTypeCount; i++) {
    VkMemoryType memory_type = memory_properties.memoryTypes[i];
    if (valid_heaps[memory_type.heapIndex] &&
        (memory_type.propertyFlags & choose_info.properties) == choose_info.properties &&
        (1 << i & choose_info.memory_types)) {
      return i;
    }
  }

  throw MemoryTypeNotFoundException();
}

} // namespace vk
