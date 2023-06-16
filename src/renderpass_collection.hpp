#pragma once
#include "vk/vulkan.hpp"

using namespace std;

class RenderPassCollection {
private:
public:
  RenderPassCollection() = delete;

  static VkRenderPass CreateFirstRenderPass(vk::Device *device, VkFormat surface_format, VkFormat texture_format);
};
