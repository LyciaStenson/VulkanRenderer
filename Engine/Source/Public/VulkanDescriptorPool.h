#pragma once

#include <vector>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VulkanDevice* device, size_t meshCount);
		~VulkanDescriptorPool();

		VkDescriptorPool Get() const;

	private:
		VkDescriptorPool descriptorPool;

		VulkanDevice* device;

		void CreateDescriptorPool(size_t meshCount);
	};
}