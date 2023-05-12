#pragma once
#include "exception.hpp"
#include "physical_device.hpp"
#include "queue.hpp"
#include "tools.hpp"
#include <algorithm>
#include <vulkan/vulkan_core.h>

using namespace std;

namespace vk {

class DeviceMemory;

struct DeviceCreateInfo {
  struct QueueRequest {
    VkQueueFlags flags;
    Queue *queue;
  };

  vector<QueueRequest> queue_requests;
};

class Device {
private:
  VkDevice handle;
  shared_ptr<PhysicalDevice> physical_device;

  static constexpr float queue_priority = 1;

  vector<VkDeviceQueueCreateInfo>
  GenerateQueueCreateInfos(vector<DeviceCreateInfo::QueueRequest> &request,
                           vector<uint32_t> &queues_family_indices);

public:
  Device(shared_ptr<PhysicalDevice> physical_device,
         DeviceCreateInfo &create_info);
  Device(Device &) = delete;
  Device &operator=(Device &) = delete;
  ~Device();

  void Dispose();
  PhysicalDevice &GetPhysicalDevice();
  VkDevice GetHandle();
};

} // namespace vk
