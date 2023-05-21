#pragma once
#include "device.hpp"
#include "exception.hpp"
#include "glm/glm.hpp"
#include "image_barrier.hpp"
#include "memory_object.hpp"
#include "queue.hpp"
#include "templates.hpp"
#include <vulkan/vulkan.h>

using namespace std;

namespace vk {

class DeviceMemory;

struct ImageCreateInfo {
  glm::ivec2 size;
  VkFormat format;
  VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

class Image : public MemoryObject {
private:
  Device *device;
  VkImage handle;
  DeviceMemory *memory;

  VkExtent3D extent;
  VkFormat format;
  VkImageLayout current_layout;

public:
  Image(Device &device, ImageCreateInfo &create_info);
  Image(Image &) = delete;
  Image &operator=(Image &) = delete;
  ~Image();

  VkImage GetHandle();
  void Destroy();

  VkImageLayout GetLayout();
  VkImageLayout ChangeLayout(VkImageLayout new_layout);

  VkExtent3D GetExtent();
  VkFormat GetFormat();
  
  friend DeviceMemory;
};

} // namespace vk
