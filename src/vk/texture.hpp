#pragma once
#include "command_buffer.hpp"
#include "device.hpp"
#include "device_memory.hpp"
#include "image_view.hpp"
#include "staging_buffer.hpp"
#include <span>

using namespace std;

namespace vk {

class Texture {
private:
  Device *device;
  unique_ptr<Image> image;
  unique_ptr<DeviceMemory> memory;

public:
  Texture(Device *device);
  Texture(Texture &) = delete;
  Texture &operator=(Texture &) = delete;
  ~Texture();

  void Destroy();

  void LoadImage(char *image_data, glm::ivec2 image_size,
                 CommandBuffer *command_buffer, Queue graphics_queue);
  unique_ptr<ImageView> CreateImageView();
};

} // namespace vk
