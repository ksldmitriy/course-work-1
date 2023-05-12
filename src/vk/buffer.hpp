#pragma once
#include "exception.hpp"
#include "queue.hpp"
#include <vulkan/vulkan.h>
#include "device.hpp"

using namespace std;

namespace vk {

class DeviceMemory;

struct BufferCreateInfo {
  VkDeviceSize size;
  VkBufferUsageFlags usage;
  Queue queue;
};

class Buffer {
private:
  Device* device;
  VkBuffer handle;
  DeviceMemory *memory;
  bool is_binded;

  VkDeviceSize size;
  VkMemoryRequirements requirements;

public:
  Buffer(Device& device, BufferCreateInfo &create_info);
  Buffer(Buffer &) = delete;
  Buffer &operator=(Buffer &) = delete;
  ~Buffer();

  void *Map();
  void Flush();
  void Unmap();
  VkBuffer GetHandle();
  void Destroy();
  VkMemoryRequirements GetMemoryRequirements();
  uint32_t GetMemoryTypes();
  VkDeviceSize GetSize();

  friend DeviceMemory;
};

} // namespace vk
