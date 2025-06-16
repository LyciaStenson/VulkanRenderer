#include <Vulkan/GlobalDescriptorSetManager.h>

#include <array>
#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Vulkan/UniformBuffer.h>
#include <Vulkan/StorageBuffer.h>
#include <Core/CameraUBO.h>
#include <Core/PointLightData.h>

using namespace VulkanRenderer;

GlobalDescriptorSetManager::GlobalDescriptorSetManager(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
	: device(device)
{
	CreateBuffers();
	CreateDescriptorSets(descriptorSetLayout, descriptorPool);
}

GlobalDescriptorSetManager::~GlobalDescriptorSetManager()
{

}

const std::vector<VkDescriptorSet>& GlobalDescriptorSetManager::GetDescriptorSets() const
{
	return descriptorSets;
}

void GlobalDescriptorSetManager::UpdateCamera(uint32_t frameIndex, const CameraUBO& cameraUBO)
{
	memcpy(cameraBuffers[frameIndex].GetMappedData(), &cameraUBO, sizeof(cameraUBO));
}

void GlobalDescriptorSetManager::UpdatePointLights(uint32_t frameIndex, const std::vector<PointLightData>& pointLights)
{
	VkDeviceSize size = sizeof(PointLightData) * pointLights.size();
	if (size > 0)
	{
		pointLightBuffers[frameIndex].UploadData(pointLights.data(), size);

		PointLightMetaUBO metaUBO{};
		metaUBO.count = static_cast<uint32_t>(pointLights.size());
		memcpy(pointLightMetaBuffers[frameIndex].GetMappedData(), &metaUBO, sizeof(metaUBO));
	}
}

void GlobalDescriptorSetManager::CreateBuffers()
{
	VkDeviceSize cameraSize = sizeof(CameraUBO);
	VkDeviceSize pointLightBufferSize = sizeof(PointLightData) * MAX_POINT_LIGHTS;
	VkDeviceSize pointLightMetaBufferSize = sizeof(PointLightMetaUBO) * MAX_POINT_LIGHTS;
	
	cameraBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	pointLightBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	pointLightMetaBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	
	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		cameraBuffers.emplace_back(device, cameraSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		pointLightBuffers.emplace_back(device, pointLightBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		pointLightMetaBuffers.emplace_back(device, cameraSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void GlobalDescriptorSetManager::CreateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
{
	VkDevice logicalDevice = device->GetLogical();
	
	std::vector<VkDescriptorSetLayout> layouts(VulkanConfig::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		std::cerr << "Failed to allocate global descriptor sets" << std::endl;
		return;
	}

	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo cameraInfo{};
		cameraInfo.buffer = cameraBuffers[i].Get();
		cameraInfo.offset = 0;
		cameraInfo.range = sizeof(CameraUBO);

		VkDescriptorBufferInfo pointLightsInfo{};
		pointLightsInfo.buffer = pointLightBuffers[i].Get();
		pointLightsInfo.offset = 0;
		pointLightsInfo.range = VK_WHOLE_SIZE;
		//pointLightsInfo.range = sizeof(PointLightData) * MAX_POINT_LIGHTS;

		VkDescriptorBufferInfo pointLightsMetaInfo{};
		pointLightsMetaInfo.buffer = pointLightMetaBuffers[i].Get();
		pointLightsMetaInfo.offset = 0;
		pointLightsMetaInfo.range = sizeof(PointLightMetaUBO);
		
		std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &cameraInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &pointLightsInfo;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = descriptorSets[i];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pBufferInfo = &pointLightsMetaInfo;

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}