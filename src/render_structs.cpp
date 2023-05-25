#include "render_structs.hpp"

VkVertexInputBindingDescription
SpriteVertex::GetBindingDescription(uint32_t binding) {
  VkVertexInputBindingDescription binding_description{};
  binding_description.binding = binding;
  binding_description.stride = sizeof(SpriteVertex);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

vector<VkVertexInputAttributeDescription>
SpriteVertex::GetAttributeDescriptions(uint32_t binding, uint32_t &location) {
  vector<VkVertexInputAttributeDescription> attribute_descriptions(2);
  attribute_descriptions[0].binding = binding;
  attribute_descriptions[0].location = location++;
  attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(SpriteVertex, pos);

  attribute_descriptions[1].binding = binding;
  attribute_descriptions[1].location = location++;
  attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(SpriteVertex, tex);

  return attribute_descriptions;
}

VkVertexInputBindingDescription
MeshVertex::GetBindingDescription(uint32_t binding) {
  VkVertexInputBindingDescription binding_description{};
  binding_description.binding = binding;
  binding_description.stride = sizeof(MeshVertex);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

vector<VkVertexInputAttributeDescription>
MeshVertex::GetAttributeDescriptions(uint32_t binding, uint32_t &location) {
  vector<VkVertexInputAttributeDescription> attribute_descriptions(1);
  attribute_descriptions[0].binding = binding;
  attribute_descriptions[0].location = location++;
  attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(MeshVertex, pos);
  return attribute_descriptions;
}

VkVertexInputBindingDescription
InstanceData::GetBindingDescription(uint32_t binding) {
  VkVertexInputBindingDescription binding_description{};
  binding_description.binding = binding;
  binding_description.stride = sizeof(InstanceData);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

  return binding_description;
}

vector<VkVertexInputAttributeDescription>
InstanceData::GetAttributeDescriptions(uint32_t binding, uint32_t &location) {
  vector<VkVertexInputAttributeDescription> attribute_descriptions(2);
  attribute_descriptions[0].binding = binding;
  attribute_descriptions[0].location = location++;
  attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(Transforn2D, pos);

  attribute_descriptions[1].binding = binding;
  attribute_descriptions[1].location = location++;
  attribute_descriptions[1].format = VK_FORMAT_R32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(Transforn2D, rot);

  return attribute_descriptions;
}

VkVertexInputBindingDescription
UniformData::GetBindingDescription(uint32_t binding) {
  VkVertexInputBindingDescription binding_description{};
  binding_description.binding = binding;
  binding_description.stride = sizeof(UniformData);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

  return binding_description;
}

vector<VkVertexInputAttributeDescription>
UniformData::GetAttributeDescriptions(uint32_t binding, uint32_t &location) {
  vector<VkVertexInputAttributeDescription> attribute_descriptions(2);
  attribute_descriptions[0].binding = binding;
  attribute_descriptions[0].location = location++;
  attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(UniformData, scale);

  attribute_descriptions[1].binding = binding;
  attribute_descriptions[1].location = location++;
  attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(UniformData, pos);

  return attribute_descriptions;
}
