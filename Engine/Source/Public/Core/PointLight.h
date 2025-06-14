#pragma once

#include <string>

#include <Core/SceneObject.h>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	
	class PointLight : public SceneObject
	{
	public:
		PointLight(const std::string& name, VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout);
		~PointLight();

		void UpdateUniformBuffer(uint32_t currentImage);

	protected:
		VulkanDevice* device;

		VkDescriptorSetLayout descriptorSetLayout;
		
		std::vector<VkDescriptorSet> descriptorSets;

		std::vector<VulkanUniformBuffer> uniformBuffers;

		void CreateUniformBuffers();
		void CreateUniformDescriptorSets(VkDescriptorPool descriptorPool);
	};
}