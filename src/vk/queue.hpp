#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>

using namespace std;

namespace vk {

class Queue {
private:
  VkQueue handle;
  uint32_t family;
  
public:
  Queue() = default;
  Queue(VkQueue handle, uint32_t family);

  VkQueue GetHandle();
  uint32_t GetFamily();
};

} // namespace vk
