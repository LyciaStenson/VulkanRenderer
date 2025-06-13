#pragma once

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanSwapChain;

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VulkanDevice* device, VulkanSwapChain* swapChain);
		~VulkanRenderPass();

		VkRenderPass Get() const;

		void Begin(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void End(VkCommandBuffer commandBuffer);

	private:
		VkRenderPass renderPass;

		VulkanDevice* device;
		VulkanSwapChain* swapChain;

		void CreateRenderPass(VkFormat swapChainImageFormat);
	};
}