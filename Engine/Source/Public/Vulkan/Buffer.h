#pragma once

#include <volk.h>
#include <vk_mem_alloc.h>

namespace VulkanRenderer
{
	class VulkanDevice;

	class VulkanBuffer
	{
	public:
		VulkanBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
		~VulkanBuffer();

		VkBuffer Get() const;
		VmaAllocation GetAllocation() const;

		void* Map();
		void Unmap();

	private:
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceSize size = 0;

		VmaAllocation allocation = VK_NULL_HANDLE;

		VulkanDevice* device;

		void CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
	};
}