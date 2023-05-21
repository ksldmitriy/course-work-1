#pragma once
#include "device.hpp"
#include <vector>
#include <vulkan/vulkan.h>
#include "exception.hpp"
#include "templates.hpp"
#include "command_buffer.hpp"
#include <memory>

using namespace std;

namespace vk {

class CommandPool {
private:
  VkCommandPool handle;
  Device *device;
  Queue queue;

  VkFence solo_execute_fence;
  
  uint32_t capacity;
  uint32_t size;

  void DisposeCommandBufferCallback();
  void CreateSoloExecuteFence();
  
public:
  CommandPool(Device &device, Queue& queue, uint32_t capacity);
  CommandPool(CommandPool &) = delete;
  CommandPool &operator=(CommandPool &) = delete;
  ~CommandPool();

  void Dispose();
  unique_ptr<CommandBuffer> AllocateCommandBuffer(CommandBufferLevel level);
  VkCommandPool GetHandle();

  friend class CommandBuffer;
};

} // namespace vk
