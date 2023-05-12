#include "queue.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace vk {

Queue::Queue(VkQueue handle, uint32_t family) {
  this->handle = handle;
  this->family = family;
}

VkQueue Queue::GetHandle() { return handle; }

uint32_t Queue::GetFamily() { return family; }

} // namespace vk
