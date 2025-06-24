#pragma once

#include <memory>

#include <ImGui/ImGuiWindow.h>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanRenderPass;
	class VulkanTexture;
	class VulkanRenderPass;
	class Scene;
	
	class SceneWindow : public ImGuiWindow
	{
	public:
		SceneWindow(VulkanDevice* device, VkFormat colorFormat, VkFormat depthFormat, bool open = true);
		~SceneWindow() override;

		VulkanTexture* GetColorTexture() const;

		VkFramebuffer GetFramebuffer() const;
		VkExtent2D GetExtent() const;
		
		void BeginRenderPass(VkCommandBuffer commandBuffer);
		void EndRenderPass(VkCommandBuffer commandBuffer);

		void RecreateRenderResources();

		bool ShouldResize();

	protected:
		void OnRender() override;
		
		void CreateRenderResources();
		void CleanupRenderResources();

		static ImGuiWindowProperties BuildProperties();
		
		VulkanDevice* device = nullptr;

		VulkanTexture* colorTexture = nullptr;
		VulkanTexture* depthTexture = nullptr;

		std::unique_ptr<VulkanRenderPass> renderPass;

		VkFramebuffer framebuffer = VK_NULL_HANDLE;

		VkExtent2D extent = {800, 600};
		VkFormat colorFormat;
		VkFormat depthFormat;

		ImTextureID imGuiTextureId = 0;

		unsigned int currentWidth = 800;
		unsigned int currentHeight = 600;

		bool shouldResize = false;
	};
}