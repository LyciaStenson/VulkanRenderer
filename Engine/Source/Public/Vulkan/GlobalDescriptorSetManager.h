#pragma once

#include <vector>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	class VulkanStorageBuffer;
	struct CameraUBO;
	struct PointLightData;
	struct PointLightMetaUBO;

	class GlobalDescriptorSetManager
	{
	public:
		GlobalDescriptorSetManager(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);
		~GlobalDescriptorSetManager();

		const std::vector<VkDescriptorSet>& GetDescriptorSets() const;

		void UpdateCamera(uint32_t frameIndex, const CameraUBO& cameraUBO);
		void UpdatePointLights(uint32_t frameIndex, const std::vector<PointLightData>& pointLights);

	private:
		VulkanDevice* device;
		
		std::vector<VkDescriptorSet> descriptorSets;

		std::vector<VulkanUniformBuffer> cameraBuffers;
		std::vector<VulkanStorageBuffer> pointLightBuffers;
		std::vector<VulkanUniformBuffer> pointLightMetaBuffers;
		
		void CreateBuffers();
		void CreateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);
	};
}