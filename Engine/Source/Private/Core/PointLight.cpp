#include <Core/PointLight.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Vulkan/UniformBuffer.h>
#include <Core/PointLightData.h>

using namespace VulkanRenderer;

PointLight::PointLight(const std::string& name)
	: SceneObject(name)
{

}

PointLight::~PointLight()
{

}

PointLightData PointLight::GetData() const
{
	PointLightData data;
	data.positionRadius = glm::vec4(transform.position, radius);
	data.colorIntensity = glm::vec4(color, intensity);

	return data;
}

//void PointLight::CreateUniformDescriptorSets(VkDescriptorPool descriptorPool)
//{
//	VkDevice logicalDevice = device->GetLogical();
//
//	std::vector<VkDescriptorSetLayout> layouts(VulkanConfig::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
//
//	VkDescriptorSetAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//	allocInfo.descriptorPool = descriptorPool;
//	allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
//	allocInfo.pSetLayouts = layouts.data();
//
//	// Allocate a descriptor set for each frame in flight
//	descriptorSets.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
//	if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
//	{
//		std::cerr << "Failed to allocate mesh descriptor sets" << std::endl;
//		return;
//	}
//
//	// Update the descriptor set for each frame in flight
//	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
//	{
//		VkDescriptorBufferInfo bufferInfo{};
//		bufferInfo.buffer = uniformBuffers[i].Get();
//		bufferInfo.offset = 0;
//		bufferInfo.range = sizeof(PointLightsUBO);
//
//		VkWriteDescriptorSet descriptorWrite{};
//		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//		descriptorWrite.dstSet = descriptorSets[i];
//		descriptorWrite.dstBinding = 0;
//		descriptorWrite.dstArrayElement = 0;
//		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		descriptorWrite.descriptorCount = 1;
//		descriptorWrite.pBufferInfo = &bufferInfo;
//
//		vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, nullptr);
//	}
//}