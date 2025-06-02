#pragma once

#include <vector>
#include <optional>

#include <GLFW/glfw3.h>

#include <volk.h>

#include <VulkanHelpers.h>

namespace VulkanRenderer
{
	class VulkanDevice
	{
	public:
		VulkanDevice(VkInstance instance, VkSurfaceKHR surface);
		~VulkanDevice();

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags);

		VkCommandBuffer BeginSingleTimeCommands() const;
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

		VkDevice GetLogical() const;
		VkPhysicalDevice GetPhysical() const;

		std::vector<VkCommandBuffer> commandBuffers;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		uint32_t graphicsQueueFamily;

	private:
		VkDevice logicaldevice;
		VkPhysicalDevice physicalDevice;

		VkInstance instance;
		VkSurfaceKHR surface;

		VkCommandPool commandPool;

		void SelectPhysicalDevice();
		void CreateLogicalDevice();

		void CreateCommandPool();
		void CreateCommandBuffers();
	};
}