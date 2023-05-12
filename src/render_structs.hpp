#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

using namespace std;

struct Vertex {
  glm::fvec2 pos;
  glm::fvec3 color;

  static VkVertexInputBindingDescription
  GetBindingDescription(uint32_t binding);
  static vector<VkVertexInputAttributeDescription>
  GetAttributeDescriptions(uint32_t binding, uint32_t location);
};

struct InstanceData {
  glm::fvec2 pos;
  float rot;

  static VkVertexInputBindingDescription
  GetBindingDescription(uint32_t binding);
  static vector<VkVertexInputAttributeDescription>
  GetAttributeDescriptions(uint32_t binding, uint32_t location);
};

struct UniformData {
  glm::fvec2 scale;
  glm::fvec2 pos;

  static VkVertexInputBindingDescription
  GetBindingDescription(uint32_t binding);
  static vector<VkVertexInputAttributeDescription>
  GetAttributeDescriptions(uint32_t binding, uint32_t location);
};
