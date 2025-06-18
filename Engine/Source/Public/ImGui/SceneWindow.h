#pragma once

#include <ImGui/ImGuiWindow.h>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanRenderPass;
	class VulkanTexture;
	
	class SceneWindow : public ImGuiWindow
	{
	public:
		SceneWindow(VulkanDevice* device, VulkanRenderPass* renderPass, VkFormat colorFormat, VkFormat depthFormat, bool open = true);
		~SceneWindow() override;

	protected:
		void OnRender() override;

		void CreateRenderResources(VkFormat colorFormat, VkFormat depthFormat);
		
		VulkanDevice* device = nullptr;
		VulkanRenderPass* renderPass = nullptr;

		VulkanTexture* colorTexture = nullptr;
		VulkanTexture* depthTexture = nullptr;

		VkFramebuffer framebuffer = VK_NULL_HANDLE;

		VkExtent2D extent = { 800, 600 };
	};
}