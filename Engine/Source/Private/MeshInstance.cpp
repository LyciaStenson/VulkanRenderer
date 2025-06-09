#include <MeshInstance.h>

#include <iostream>

#include <VulkanConfig.h>
#include <VulkanDevice.h>
#include <VulkanUniformBuffer.h>
#include <Mesh.h>
#include <MeshUBO.h>

using namespace VulkanRenderer;

MeshInstance::MeshInstance(const std::string& name, const Transform& transform, std::shared_ptr<Mesh> mesh, VulkanDevice* device, VkDescriptorPool descriptorPool)
	: SceneObject(name, transform), device(device), mesh(mesh)
{
	CreateUniformBuffers();
	CreateUniformDescriptorSets(descriptorPool);
}

MeshInstance::~MeshInstance()
{

}

std::shared_ptr<const Mesh> MeshInstance::GetMesh() const
{
	return mesh;
}

const std::vector<VkDescriptorSet>& MeshInstance::GetUniformDescriptorSets() const
{
	return uniformDescriptorSets;
}

void MeshInstance::CreateUniformDescriptorSets(VkDescriptorPool descriptorPool)
{
	VkDevice logicalDevice = device->GetLogical();

	std::vector<VkDescriptorSetLayout> layouts(VulkanConfig::MAX_FRAMES_IN_FLIGHT, mesh->GetUniformDescriptorSetLayout());

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	// Allocate a descriptor set for each frame in flight
	uniformDescriptorSets.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, uniformDescriptorSets.data()) != VK_SUCCESS)
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

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = uniformDescriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		//descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrites[1].dstSet = uniformDescriptorSets[i];
		//descriptorWrites[1].dstBinding = 1;
		//descriptorWrites[1].dstArrayElement = 0;
		//descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//descriptorWrites[1].descriptorCount = 1;
		//descriptorWrites[1].pImageInfo = &mesh->GetBaseColorDescriptorInfo();

		//descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrites[2].dstSet = uniformDescriptorSets[i];
		//descriptorWrites[2].dstBinding = 2;
		//descriptorWrites[2].dstArrayElement = 0;
		//descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//descriptorWrites[2].descriptorCount = 1;
		//descriptorWrites[2].pImageInfo = &mesh->GetRoughnessDescriptorInfo();

		//descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrites[3].dstSet = uniformDescriptorSets[i];
		//descriptorWrites[3].dstBinding = 3;
		//descriptorWrites[3].dstArrayElement = 0;
		//descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//descriptorWrites[3].descriptorCount = 1;
		//descriptorWrites[3].pImageInfo = &mesh->GetMetallicDescriptorInfo();

		vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, nullptr);
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