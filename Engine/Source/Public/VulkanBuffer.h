#pragma once

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;

	class VulkanBuffer
	{
	public:
		VulkanBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
		~VulkanBuffer();

		VkBuffer Get() const;
		VkDeviceMemory GetMemory() const;

	private:
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDeviceSize size = 0;

		VulkanDevice* device;

		void CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
	};
}