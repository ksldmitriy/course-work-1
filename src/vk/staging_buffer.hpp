#pragma once
#include "barrier.hpp"
#include "buffer.hpp"
#include "command_buffer.hpp"
#include "device_memory.hpp"
#include <memory>
#include <span>
#include <string.h>
#include <vector>
#include <vulkan/vulkan.h>

using namespace std;

namespace vk {

struct StagingBufferCreateInfo {
  CommandBuffer *command_buffer;
  Queue queue;
  VkDeviceSize size;
};

class StagingBuffer {
protected:
  unique_ptr<DeviceMemory> memory;
  unique_ptr<Buffer> buffer;

  CommandBuffer *command_buffer;
  Device *device;
  Queue queue;

  MemoryBarrier CreateLoadDataBarrier();

  void SetPreloadImageBarrier(Image* image);
  SrcImageBarrier CreateAfterloadImageBarrier(Image* image); 
  
  void CreateBuffer(VkDeviceSize size);
  void CreateMemory();

public:
  StagingBuffer(Device &device, StagingBufferCreateInfo &create_info);
  StagingBuffer(StagingBuffer &) = delete;
  StagingBuffer &operator=(StagingBuffer &) = delete;
  ~StagingBuffer();

  void Free();

  void LoadData(span<char> data);

  void CopyToBuffer(Buffer *buffer, VkDeviceSize size = VK_WHOLE_SIZE,
                    VkDeviceSize src_offet = 0, VkDeviceSize dst_offet = 0);
  SrcImageBarrier CopyToImage(Image *image, SrcImageBarrier &src_barrier);
};

} // namespace vk
