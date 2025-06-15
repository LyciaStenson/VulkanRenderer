#include <Core/PointLight.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Vulkan/UniformBuffer.h>
#include <Core/PointLightsUBO.h>

using namespace VulkanRenderer;

PointLight::PointLight(const std::string& name, VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
	: SceneObject(name), device(device), descriptorSetLayout(descriptorSetLayout)
{

}

PointLight::~PointLight()
{

}

void PointLight::CreateUniformDescriptorSets(VkDescriptorPool descriptorPool)
{
	VkDevice logicalDevice = device->GetLogical();

	std::vector<VkDescriptorSetLayout> layouts(VulkanConfig::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	// Allocate a descriptor set for each frame in flight
	descriptorSets.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		std::cerr << "Failed to allocate mesh descriptor sets" << std::endl;
		return;
	}

	// Update the descriptor set for each frame in flight
	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i].Get();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(PointLightsUBO);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, nullptr);
	}
}

void PointLight::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(PointLightsUBO);
	uniformBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		uniformBuffers.emplace_back(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void PointLight::UpdateUniformBuffer(uint32_t currentImage)
{
	//PointLightUBO ubo{};
	//ubo.position = transform.position;
	
	//memcpy(uniformBuffers[currentImage].GetMappedData(), &ubo, sizeof(ubo));
}