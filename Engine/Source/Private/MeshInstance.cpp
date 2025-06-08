#include <MeshInstance.h>

#include <iostream>

#include <VulkanConfig.h>
#include <VulkanDevice.h>
#include <VulkanUniformBuffer.h>
#include <Mesh.h>
#include <MeshUBO.h>

using namespace VulkanRenderer;

MeshInstance::MeshInstance(VulkanDevice* device, VkDescriptorPool descriptorPool, std::shared_ptr<Mesh> mesh, const Transform& transform, const std::string& name)
	: SceneObject(name, transform), device(device), mesh(mesh)
{
	CreateUniformBuffers();
	CreateDescriptorSets(descriptorPool);
}

MeshInstance::~MeshInstance()
{

}

std::shared_ptr<const Mesh> MeshInstance::GetMesh() const
{
	return mesh;
}

const std::vector<VkDescriptorSet>& MeshInstance::GetDescriptorSets() const
{
	return descriptorSets;
}

void MeshInstance::CreateDescriptorSets(VkDescriptorPool descriptorPool)
{
	VkDevice logicalDevice = device->GetLogical();

	std::vector<VkDescriptorSetLayout> layouts(VulkanConfig::MAX_FRAMES_IN_FLIGHT, mesh->GetDescriptorSetLayout());

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
		bufferInfo.range = sizeof(MeshUBO);

		std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &mesh->GetBaseColorDescriptorInfo();

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = descriptorSets[i];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &mesh->GetRoughnessDescriptorInfo();

		descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[3].dstSet = descriptorSets[i];
		descriptorWrites[3].dstBinding = 3;
		descriptorWrites[3].dstArrayElement = 0;
		descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[3].descriptorCount = 1;
		descriptorWrites[3].pImageInfo = &mesh->GetMetallicDescriptorInfo();

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void MeshInstance::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(MeshUBO);
	uniformBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		uniformBuffers.emplace_back(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void MeshInstance::UpdateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent)
{
	MeshUBO ubo{};
	ubo.model = transform.GetWorldMatrix();

	memcpy(uniformBuffers[currentImage].GetMappedData(), &ubo, sizeof(ubo));
}