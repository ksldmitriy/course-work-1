#include "device.hpp"
#include "../logs.hpp"
#include "device_memory.hpp"
#include "templates.hpp"
#include <algorithm>
#include <vulkan/vulkan_core.h>

namespace vk {

Device::Device(shared_ptr<PhysicalDevice> physical_device,
               DeviceCreateInfo &create_info) {
  this->physical_device = physical_device;

  vector<uint32_t> queue_family_indices;
  vector<VkDeviceQueueCreateInfo> queue_create_infos = GenerateQueueCreateInfos(
      create_info.queue_requests, queue_family_indices);

  // device create info
  VkDeviceCreateInfo vk_create_info = device_create_info_template;
  vk_create_info.queueCreateInfoCount = queue_create_infos.size();
  vk_create_info.pQueueCreateInfos = queue_create_infos.data();

  const char *extensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  vk_create_info.enabledExtensionCount = 1;
  vk_create_info.ppEnabledExtensionNames = &extensions;

  TRACE("device create info generated");

  VkResult result = vkCreateDevice(physical_device->GetHandle(),
                                   &vk_create_info, nullptr, &handle);
  if (result) {
    throw CriticalException("cant create device");
  }

  DEBUG("device created");

  for (int i = 0; i < create_info.queue_requests.size(); i++) {
    uint32_t family = queue_family_indices[i];
    VkQueue queue;
    vkGetDeviceQueue(handle, family, 0, &queue);
    *create_info.queue_requests[i].queue = Queue(queue, family);
  }

  TRACE("device queues returned");
}

Device::~Device() {
  if (handle == VK_NULL_HANDLE) {
    return;
  }

  Dispose();
}

void Device::Dispose() {
  vkDestroyDevice(handle, nullptr);
  handle = VK_NULL_HANDLE;
  
  DEBUG("device disposed");
}

vector<VkDeviceQueueCreateInfo> Device::GenerateQueueCreateInfos(
    vector<DeviceCreateInfo::QueueRequest> &request,
    vector<uint32_t> &queues_family_indices) {
  vector<uint32_t> queue_families;

  queue_families.clear();

  for (int i = 0; i < request.size(); i++) {
    uint32_t family = physical_device->ChooseQueueFamily(request[i].flags);

    queue_families.push_back(family);
    queues_family_indices.push_back(family);
  }

  auto end = unique(queue_families.begin(), queue_families.end());
  queue_families.resize(distance(queue_families.begin(), end));

  vector<VkDeviceQueueCreateInfo> create_infos;

  for (int i = 0; i < queue_families.size(); i++) {
    VkDeviceQueueCreateInfo create_info = vk::queue_create_info_template;
    create_info.queueFamilyIndex = queue_families[i];
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &queue_priority;

    create_infos.push_back(create_info);
  }

  return create_infos;
}

PhysicalDevice &Device::GetPhysicalDevice() { return *physical_device; }

VkDevice Device::GetHandle() { return handle; }

} // namespace vk
