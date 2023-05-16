#include "memory_object.hpp"

namespace vk {

VkMemoryRequirements MemoryObject::GetMemoryRequirements() {
  return requirements;
}

uint32_t MemoryObject::GetMemoryTypes() { return requirements.memoryTypeBits; }

} // namespace vk
