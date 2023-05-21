#include "render_structs.hpp"

VkVertexInputBindingDescription
Vertex::GetBindingDescription(uint32_t binding) {
  VkVertexInputBindingDescription binding_description{};
  binding_description.binding = binding;
  binding_description.stride = sizeof(Vertex);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

vector<VkVertexInputAttributeDescription>
Vertex::GetAttributeDescriptions(uint32_t binding, uint32_t &location) {
  vector<VkVertexInputAttributeDescription> attribute_descriptions(2);
  attribute_descriptions[0].binding = binding;
  attribute_descriptions[0].location = location++;
  attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(Vertex, pos);

  attribute_descriptions[1].binding = binding;
  attribute_descriptions[1].location = location++;
  attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(Vertex, tex);

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
  attribute_descriptions[0].offset = offsetof(InstanceData, pos);
 
  attribute_descriptions[1].binding = binding;
  attribute_descriptions[1].location = location++;
  attribute_descriptions[1].format = VK_FORMAT_R32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(InstanceData, rot);

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
