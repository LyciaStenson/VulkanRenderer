#pragma once

#include <memory>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapChain;
	class VulkanRenderPass;
	class VulkanDescriptorSetLayoutManager;
	class GlobalDescriptorSetManager;
	class VulkanPipeline;
	class VulkanDescriptorPool;
	class VulkanSync;
	class GlfwWindow;
	class Scene;
	class RenderTarget;
	
	class Renderer
	{
	public:
		Renderer(GlfwWindow* glfwWindow);
		~Renderer();
		
		VulkanDevice* GetDevice() const;
		VulkanInstance* GetInstance() const;
		VulkanSwapChain* GetSwapChain() const;
		VulkanRenderPass* GetRenderPass() const;
		VulkanDescriptorSetLayoutManager* GetDescriptorSetLayoutManager() const;
		GlobalDescriptorSetManager* GetGlobalDescriptorSetManager() const;
		VulkanDescriptorPool* GetDescriptorPool() const;
		
		void SetRenderTarget(RenderTarget* renderTarget);

		void DrawFrame(Scene* scene);

		void DrawScene(Scene* scene, VkCommandBuffer commandBuffer, VkExtent2D extent);

		void FramebufferResized();

	private:
		void RecreateSwapChain();

		std::unique_ptr<VulkanInstance> instance;
		std::unique_ptr<VulkanDevice> device;
		std::unique_ptr<VulkanSwapChain> swapChain;
		std::unique_ptr<VulkanRenderPass> renderPass;
		std::unique_ptr<VulkanDescriptorSetLayoutManager> descriptorSetLayoutManager;
		std::unique_ptr<VulkanDescriptorPool> descriptorPool;
		std::unique_ptr<GlobalDescriptorSetManager> globalDescriptorSetManager;
		std::unique_ptr<VulkanSync> sync;

		std::unique_ptr<VulkanPipeline> opaquePipeline;
		std::unique_ptr<VulkanPipeline> transparentPipeline;
		
		RenderTarget* renderTarget = nullptr;

		GlfwWindow* glfwWindow = nullptr;

		int currentFrame = 0;

		bool framebufferResized = false;
	};
}