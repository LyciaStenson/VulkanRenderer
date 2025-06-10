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
		VkDescriptorSetLayout GetMaterialDescriptorSetLayout() const;

	private:
		void CreateCameraDescriptorSetLayout();
		void CreateMeshDescriptorSetLayout();
		void CreateMaterialDescriptorSetLayout();

		VkDescriptorSetLayout cameraDescriptorSetLayout;
		VkDescriptorSetLayout meshDescriptorSetLayout;
		VkDescriptorSetLayout materialDescriptorSetLayout;

		VulkanDevice* device;
	};
}