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

		VkDescriptorSetLayout GetGlobalDescriptorSetLayout() const;
		VkDescriptorSetLayout GetMeshDescriptorSetLayout() const;
		VkDescriptorSetLayout GetMaterialDescriptorSetLayout() const;

	private:
		void CreateGlobalDescriptorSetLayout();
		void CreateMeshDescriptorSetLayout();
		void CreateMaterialDescriptorSetLayout();

		VkDescriptorSetLayout globalDescriptorSetLayout;
		VkDescriptorSetLayout meshDescriptorSetLayout;
		VkDescriptorSetLayout materialDescriptorSetLayout;
		
		VulkanDevice* device;
	};
}