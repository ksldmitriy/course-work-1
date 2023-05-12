#include "shader_module.hpp"

namespace vk {

ShaderModule::ShaderModule(Device &device, fs::path filepath) {
  this->device = &device;

  vector<char> code = ReadFile(filepath);

  VkShaderModuleCreateInfo create_info = shader_module_create_info_template;
  create_info.codeSize = code.size();
  create_info.pCode = (uint32_t *)code.data();

  VkResult result =
      vkCreateShaderModule(device.GetHandle(), &create_info, nullptr, &handle);
  if (result) {
    throw CriticalException("cant create shader module");
  }

  TRACE("shader module from {0} file created", filepath.string());
}

ShaderModule::~ShaderModule() {
  vkDestroyShaderModule(device->GetHandle(), handle, nullptr);

  TRACE("shader module destoyed");
}

VkShaderModule ShaderModule::GetHandle() { return handle; }

vector<char> ShaderModule::ReadFile(fs::path filepath) {
  ifstream file(filepath, ios::binary);

  if (!file) {
    throw CriticalException("cant open file \"" + filepath.string() + "\"");
  }

  file.seekg(0, ios::end);
  size_t filesize = file.tellg();
  file.seekg(0, ios::beg);

  vector<char> data(filesize);
  file.read(data.data(), filesize);

  return data;
}

} // namespace vk
