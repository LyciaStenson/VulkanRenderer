#include <Camera.h>

#include <iostream>

#include <VulkanConfig.h>
#include <CameraUBO.h>

#include <VulkanDevice.h>

using namespace VulkanRenderer;

Camera::Camera(const std::string& name, VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout)
	: SceneObject(name), device(device), descriptorSetLayout(descriptorSetLayout)
{
	CreateUniformBuffers();
}

Camera::~Camera()
{

}

void Camera::CreateDescriptorSets(VkDescriptorPool descriptorPool)
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
		std::cerr << "Failed to allocate camera descriptor sets" << std::endl;
		return;
	}

	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i].Get();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(CameraUBO);
		
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

void Camera::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(CameraUBO);
	uniformBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		uniformBuffers.emplace_back(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void Camera::UpdateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent)
{
	CameraUBO ubo{};

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform.position);
	glm::mat4 rotation = glm::mat4_cast(transform.rotation);
	glm::mat4 world = translation * rotation;

	ubo.view = glm::inverse(world);
	ubo.proj = glm::perspective(glm::radians(fov), (float)swapChainExtent.width / (float)swapChainExtent.height, 0.01f, 100.0f);
	ubo.proj[1][1] *= -1;
	
	memcpy(uniformBuffers[currentImage].GetMappedData(), &ubo, sizeof(ubo));
}