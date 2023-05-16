#pragma once
#include <vulkan/vulkan.h>

using namespace std;

namespace vk {

class MemoryObject {
protected:

  VkMemoryRequirements requirements;
public:
  VkMemoryRequirements GetMemoryRequirements();
  uint32_t GetMemoryTypes();
};

} // namespace vk
