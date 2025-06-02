#pragma once

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;

	class ImGuiDescriptorPool
	{
	public:
		ImGuiDescriptorPool(VulkanDevice* device);
		~ImGuiDescriptorPool();

		VkDescriptorPool Get() const;

	private:
		VkDescriptorPool descriptorPool;

		VulkanDevice* device;

		void CreateDescriptorPool();
	};
}