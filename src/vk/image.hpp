#pragma once
#include "device.hpp"
#include "exception.hpp"
#include "memory_object.hpp"
#include "queue.hpp"
#include <vulkan/vulkan.h>
#include "templates.hpp"
#include "glm/glm.hpp"

using namespace std;

namespace vk {

class DeviceMemory;

struct ImageCreateInfo {
  glm::ivec2 size;
};

class Image : public MemoryObject {
private:
  Device *device;
  VkImage handle;
  DeviceMemory *memory;

  VkDeviceSize size;

public:
  Image(Device &device, ImageCreateInfo &create_info);
  Image(Image &) = delete;
  Image &operator=(Image &) = delete;
  ~Image();

  VkImage GetHandle();
  void Destroy();
  VkMemoryRequirements GetMemoryRequirements();
  uint32_t GetMemoryTypes();

  friend DeviceMemory;
};

} // namespace vk
