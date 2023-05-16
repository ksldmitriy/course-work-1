#pragma once
#include <vulkan/vulkan.h>

using namespace std;

namespace vk {

class MemoryObject {
private:
public:
  virtual VkMemoryRequirements GetMemoryRequirements() = 0;
  virtual uint32_t GetMemoryTypes() = 0;
};

} // namespace vk
