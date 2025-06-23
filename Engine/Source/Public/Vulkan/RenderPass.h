#pragma once

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VulkanDevice* device, VkFormat colorFormat, VkFormat depthFormat, VkImageLayout finalColorLayout);
		~VulkanRenderPass();

		VkRenderPass Get() const;

		void Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent);
		void End(VkCommandBuffer commandBuffer);

		void BeginCommandBuffer(VkCommandBuffer commandBuffer);
		void EndCommandBuffer(VkCommandBuffer commandBuffer);

	private:
		VkRenderPass renderPass;

		VulkanDevice* device;

		void Create(VkFormat colorFormat, VkFormat depthFormat, VkImageLayout finalColorLayout);
	};
}