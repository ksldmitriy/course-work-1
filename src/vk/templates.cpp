#include "templates.hpp"

namespace vk {

VkDeviceQueueCreateInfo queue_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0};

VkDeviceCreateInfo device_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = nullptr,
    .enabledExtensionCount = 0,
    .ppEnabledExtensionNames = nullptr,
    .pEnabledFeatures = nullptr};

VkImageSubresourceRange image_subresource_range_template = {
    .baseMipLevel = 0,
    .levelCount = 1,
    .baseArrayLayer = 0,
    .layerCount = 1,
};

VkMemoryBarrier memory_barrier_template = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER, .pNext = nullptr};

VkBufferMemoryBarrier buffer_barrier_template = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
    .pNext = nullptr,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .offset = 0,
    .size = VK_WHOLE_SIZE};

VkCommandBufferBeginInfo command_buffer_begin_info_template = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = nullptr,
    .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT};

VkPresentInfoKHR present_info_template = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, .pNext = nullptr};

VkRenderPassBeginInfo render_pass_begin_info_template = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, .pNext = nullptr};

VkImageMemoryBarrier image_memory_barrier_template = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, .pNext = 0};

VkCommandPoolCreateInfo command_pool_create_info_template{
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT};

VkSubmitInfo submit_info_template = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                     .pNext = nullptr,
                                     .waitSemaphoreCount = 0,
                                     .signalSemaphoreCount = 0};

VkShaderModuleCreateInfo shader_module_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
};

VkComputePipelineCreateInfo compute_pipeline_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
};

VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .pSpecializationInfo = nullptr};

VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0};

VkPipelineLayoutCreateInfo pipeline_layout_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .pushConstantRangeCount = 0};

VkDescriptorPoolCreateInfo descriptor_pool_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT};

VkDescriptorSetAllocateInfo descriptor_set_allocate_info_template = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, .pNext = nullptr};

VkWriteDescriptorSet write_descriptor_set_template = {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .pNext = nullptr,
    .dstArrayElement = 0,
    .descriptorCount = 0,
    .pImageInfo = nullptr,
    .pBufferInfo = nullptr,
    .pTexelBufferView = nullptr};

VkSemaphoreCreateInfo semaphore_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0};

VkFenceCreateInfo fence_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .pNext = nullptr, .flags = 0};

VkSwapchainCreateInfoKHR swapchain_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = nullptr,
    .flags = 0,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr};

VkRenderPassCreateInfo render_pass_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0};

VkAttachmentDescription attachment_description_template = {
    .flags = 0,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE};

VkSubpassDescription subpass_description_template = {
    .flags = 0,
    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    .inputAttachmentCount = 0,
    .pInputAttachments = nullptr,
    .colorAttachmentCount = 0,
    .pColorAttachments = nullptr,
    .pResolveAttachments = nullptr,
    .pDepthStencilAttachment = nullptr,
    .preserveAttachmentCount = 0,
    .pPreserveAttachments = nullptr};

VkImageViewCreateInfo image_view_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0};

VkFramebufferCreateInfo framebuffer_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0};

VkPipelineVertexInputStateCreateInfo vertex_input_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0};

VkPipelineInputAssemblyStateCreateInfo
    pipeline_input_assembly_create_info_template = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .primitiveRestartEnable = false};

VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info_template{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .viewportCount = 1,
    .scissorCount = 1};

VkPipelineRasterizationStateCreateInfo
    pipeline_rasterization_state_create_info_template = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizerDiscardEnable = VK_FALSE,
        .cullMode = VK_CULL_MODE_NONE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0,
        .depthBiasClamp = 0,
        .depthBiasSlopeFactor = 0,
        .lineWidth = 1};

VkPipelineMultisampleStateCreateInfo
    pipeline_multisample_state_create_info_template = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE};

VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0};

VkPipelineColorBlendStateCreateInfo
    pipeline_color_blend_state_create_info_template = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .blendConstants = {1, 1, 1, 1}};

VkGraphicsPipelineCreateInfo graphics_pipeline_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0};

VkCommandBufferAllocateInfo command_buffer_allocate_info_template = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, .pNext = nullptr};

VkBufferCreateInfo buffer_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

VkImageCreateInfo image_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .extent = {.width = 0, .height = 0, .depth = 1},
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

VkSamplerCreateInfo sampler_create_info_template = {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
    .magFilter = VK_FILTER_LINEAR,
    .minFilter = VK_FILTER_LINEAR,
    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
    .mipLodBias = 0.0f,
    .anisotropyEnable = VK_FALSE,
    .maxAnisotropy = 1,
    .compareEnable = VK_FALSE,
    .compareOp = VK_COMPARE_OP_ALWAYS,
    .minLod = 0.0f,
    .maxLod = 0.0f,
    .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    .unnormalizedCoordinates = VK_FALSE
    };

} // namespace vk
