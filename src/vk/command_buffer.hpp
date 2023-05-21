#pragma once
#include "device.hpp"
#include "queue.hpp"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

using namespace std;

namespace vk {

class CommandPool;

enum class CommandBufferLevel {
  primary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
  secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY
};

struct CommandBufferCreateInfo {
  CommandPool *pool;
  CommandBufferLevel level;
  Queue queue;
};

class CommandBuffer {
private:
  VkCommandBuffer handle;
  CommandPool *pool;
  Queue queue;

  CommandBuffer(CommandBufferCreateInfo &create_info);

public:
  CommandBuffer(CommandBuffer &) = delete;
  CommandBuffer &operator=(CommandBuffer &) = delete;
  ~CommandBuffer();

  VkCommandBuffer GetHandle();

  void Reset();
  void Begin();
  void End();

  void SoloExecute();

  void Dispose();

  friend class CommandPool;
};

} // namespace vk
