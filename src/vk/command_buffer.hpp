#pragma once
#include "device.hpp"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

using namespace std;

namespace vk {

class CommandPool;

enum class CommandBufferLevel {
  primary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
  secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY
};

class CommandBuffer {
private:
  VkCommandBuffer handle;
  CommandPool *pool;

  CommandBuffer(CommandPool &pool, CommandBufferLevel level);

public:
  CommandBuffer(CommandBuffer &) = delete;
  CommandBuffer &operator=(CommandBuffer &) = delete;
  ~CommandBuffer();

  VkCommandBuffer GetHandle();
  void Begin();
  void End();
  void Dispose();

  friend class CommandPool;
};

} // namespace vk
