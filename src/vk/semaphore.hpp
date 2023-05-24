#pragma once
#include <vulkan/vulkan.h>
#include "device.hpp"
#include "templates.hpp"

using namespace std;

namespace vk {

class Semaphore {
private:
  VkSemaphore handle;
  Device* device;
  
public:
  Semaphore(Device* device);
  Semaphore(Semaphore &) = delete;
  Semaphore &operator=(Semaphore &) = delete;
  ~Semaphore();
  
  void Destroy();
  const VkSemaphore& GetHandle();
};

}; // namespace vk
