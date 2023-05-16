#pragma once
#include "device.hpp"
#include "exception.hpp"
#include "memory_object.hpp"
#include "queue.hpp"
#include <vulkan/vulkan.h>
#include "templates.hpp"

using namespace std;

namespace vk {

class DeviceMemory;

struct BufferCreateInfo {
  VkDeviceSize size;
  VkBufferUsageFlags usage;
  Queue queue;
};

class Buffer : public MemoryObject {
private:
  Device *device;
  VkBuffer handle;
  DeviceMemory *memory;
  bool is_binded;

  VkDeviceSize size;

public:
  Buffer(Device &device, BufferCreateInfo &create_info);
  Buffer(Buffer &) = delete;
  Buffer &operator=(Buffer &) = delete;
  ~Buffer();

  void *Map();
  void Flush();
  void Unmap();
  VkBuffer GetHandle();
  void Destroy();
  VkDeviceSize GetSize();

  friend DeviceMemory;
};

} // namespace vk
