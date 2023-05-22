#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>
#include "transform.hpp"

using namespace std;

struct Vertex {
  glm::fvec2 pos;
  glm::fvec2 tex;

  static VkVertexInputBindingDescription
  GetBindingDescription(uint32_t binding);
  static vector<VkVertexInputAttributeDescription>
  GetAttributeDescriptions(uint32_t binding, uint32_t &location);
};

struct InstanceData {
  Transforn2D transform;

  static VkVertexInputBindingDescription
  GetBindingDescription(uint32_t binding);
  static vector<VkVertexInputAttributeDescription>
  GetAttributeDescriptions(uint32_t binding, uint32_t &location);
};

struct UniformData {
  glm::fvec2 scale;
  glm::fvec2 pos;

  static VkVertexInputBindingDescription
  GetBindingDescription(uint32_t binding);
  static vector<VkVertexInputAttributeDescription>
  GetAttributeDescriptions(uint32_t binding, uint32_t &location);
};
