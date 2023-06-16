#include "renderpass_collection.hpp"

static VkRenderPass CreateFirstRenderPass(vk::Device *device,
                                          VkFormat surface_format,
                                          VkFormat texture_format) {}

// VkAttachmentDescription color_attachment =
//     vk::attachment_description_template;
// color_attachment.format = framebuffer_format;
// color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
// color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
// color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
// color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

// VkAttachmentReference attachment_reference;
// attachment_reference.attachment = &color_attachment;
// attachment_reference.layout =

// VkSubpassDescription subpass = vk::subpass_description_template;
// subpass.colorAttachmentCount = 1;
// subpass.pColorAttachments = &color_attachment;
// subpass.

//     VkRenderPassCreateInfo create_info =
//     vk::render_pass_create_info_template;
// create_info.subpassCount = 1;
// create_info.pSubpasses = &subpass;

// VkRenderPass render_pass;

// VkResult result = vkCreateRenderPass(device->GetHandle(), &create_info,
//                                      nullptr, &render_pass);
// if (result) {
//   throw vk::CriticalException("cant create framebuffer render pass");
// }
