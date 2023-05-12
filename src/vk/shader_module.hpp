#pragma once
#include "device.hpp"
#include <fstream>
#include <filesystem>
#include <vector>
#include "templates.hpp"

using namespace std;
namespace fs = filesystem;

namespace vk {

class ShaderModule {
private:
  VkShaderModule handle;
  Device* device;
  
  vector<char> ReadFile(fs::path filepath);
public:
  ShaderModule(Device& device, fs::path filepath);
  ShaderModule(ShaderModule &) = delete;
  ShaderModule &operator=(ShaderModule &) = delete;
  ~ShaderModule();

  VkShaderModule GetHandle();
};

} // namespace vk
