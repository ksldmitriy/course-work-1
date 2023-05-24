#include "semaphore.hpp"

namespace vk {

Semaphore::Semaphore(Device *device) {
  this->device = device;

  VkSemaphoreCreateInfo semphore_create_info =
      vk::semaphore_create_info_template;

  VkResult result = vkCreateSemaphore(device->GetHandle(),
                                      &semphore_create_info, nullptr, &handle);
  if (result) {
    throw vk::CriticalException("cant create semaphore");
  }

  TRACE("semaphore created");
}

const VkSemaphore &Semaphore::GetHandle() { return handle; }

Semaphore::~Semaphore() { Destroy(); }

void Semaphore::Destroy() {
  if (handle == VK_NULL_HANDLE) {
    return;
  }

  vkDestroySemaphore(device->GetHandle(), handle, nullptr);

  handle = VK_NULL_HANDLE;

  TRACE("semaphore destroyed");
}

} // namespace vk
