#pragma once

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VulkanDevice* device, VkFormat swapChainImageFormat);
		~VulkanRenderPass();

		VkRenderPass Get() const;

	private:
		VkRenderPass renderPass;

		VulkanDevice* device;

		void CreateRenderPass(VkFormat swapChainImageFormat);
	};
}