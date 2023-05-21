#pragma once
#include "image.hpp"

using namespace std;

namespace vk {

class ImageView {
private:
  Device* device;

  VkImageView handle;

public:
  ImageView(Device* device, Image *image);
  ImageView(ImageView &) = delete;
  ImageView &operator=(ImageView &) = delete;
  ~ImageView();


  void Destroy();

  VkImageView GetHandle();
};

} // namespace vk
