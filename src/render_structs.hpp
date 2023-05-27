#pragma once
#include "transform.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

using namespace std;

struct Line {
  glm::fvec2 start;
  glm::fvec2 end;
  
};

struct Rect{
  glm::fvec2 top_left;
  glm::fvec2 bottom_right;
};

struct Circle{
  glm::fvec2 center;
  float radius;
};

struct Triangle {
  glm::fvec2 v[3];
};

struct SpriteVertex {
  glm::fvec2 pos;
  glm::fvec2 tex;

  static VkVertexInputBindingDescription
  GetBindingDescription(uint32_t binding);
  static vector<VkVertexInputAttributeDescription>
  GetAttributeDescriptions(uint32_t binding, uint32_t &location);
};

struct MeshVertex {
  glm::fvec2 pos;

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
