#pragma once
#include "exception.hpp"
#include "physical_device.hpp"
#include "tools.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

using namespace std;

namespace vk {

struct InstanceCreateInfo {
  vector<string> layers;
  vector<string> extensions;
};

class Instance {
private:
  VkInstance handle;

  void CreateInstance(InstanceCreateInfo &create_info);
  void EnumPhysicalDevices();

public:
  vector<shared_ptr<PhysicalDevice>> physical_devices;

  Instance(InstanceCreateInfo &create_info);
  Instance(Instance &) = delete;
  Instance &operator=(Instance &) = delete;
  ~Instance();

  void Dispose();
  VkInstance GetHandle();
};

} // namespace vk
