#include <ImGui/SceneWindow.h>

#include <array>
#include <iostream>

#include <Vulkan/Device.h>
#include <Vulkan/RenderPass.h>
#include <Vulkan/Texture.h>
#include <Core/Scene.h>
#include <Core/MeshInstance.h>

using namespace VulkanRenderer;

SceneWindow::SceneWindow(VulkanDevice* device, VulkanRenderPass* renderPass, VkFormat colorFormat, VkFormat depthFormat, bool open)
	: ImGuiWindow("Scene", open), device(device), renderPass(renderPass)
{
	CreateRenderResources(colorFormat, depthFormat);
}

SceneWindow::~SceneWindow()
{
	if (colorTexture)
		delete colorTexture;
	if (depthTexture)
		delete depthTexture;
	if (framebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(device->GetLogical(), framebuffer, nullptr);
	}
}

VulkanTexture* SceneWindow::GetColorTexture() const
{
	return colorTexture;
}

void SceneWindow::OnRender()
{
	ImGui::Image(imGuiTextureId, ImVec2(extent.width, extent.height));
}

void SceneWindow::CreateRenderResources(VkFormat colorFormat, VkFormat depthFormat)
{
	colorTexture = new VulkanTexture(device, extent.width, extent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	imGuiTextureId = reinterpret_cast<ImTextureID>(ImGui_ImplVulkan_AddTexture(colorTexture->GetSampler(), colorTexture->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
	
	depthTexture = new VulkanTexture(device, extent.width, extent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	std::array<VkImageView, 2> attachments =
	{
		colorTexture->GetImageView(),
		depthTexture->GetImageView()
	};
	
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass->Get();
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = extent.width;
	framebufferInfo.height = extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(device->GetLogical(), &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
	{
		std::cerr << "Failed to create framebuffers" << std::endl;
	}
}