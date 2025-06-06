#pragma once

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;

	class VulkanDescriptorSetLayoutManager
	{
	public:
		VulkanDescriptorSetLayoutManager(VulkanDevice* device);
		~VulkanDescriptorSetLayoutManager();

		VkDescriptorSetLayout GetCameraDescriptorSetLayout() const;
		VkDescriptorSetLayout GetMeshDescriptorSetLayout() const;

	private:
		void CreateCameraDescriptorSetLayout();
		void CreateMeshDescriptorSetLayout();

		VkDescriptorSetLayout cameraDescriptorSetLayout;
		VkDescriptorSetLayout meshDescriptorSetLayout;

		VulkanDevice* device;
	};
}