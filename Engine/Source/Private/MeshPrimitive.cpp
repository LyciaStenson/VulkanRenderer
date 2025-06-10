#include <MeshPrimitive.h>

#include <iostream>

#include <VulkanConfig.h>
#include <VulkanHelpers.h>
#include <VulkanDevice.h>
#include <VulkanTexture.h>
#include <VulkanBuffer.h>
#include <Vertex.h>

using namespace VulkanRenderer;

MeshPrimitive::MeshPrimitive(VulkanDevice* device, VkDescriptorSetLayout materialDescriptorSetLayout, const MeshPrimitiveInfo& info)
	: device(device), materialDescriptorSetLayout(materialDescriptorSetLayout), transparencyEnabled(info.enableTransparency)
{
	CreateVertexBuffer(info.vertices);
	CreateIndexBuffer(info.indices);
}

MeshPrimitive::~MeshPrimitive()
{
	delete indexBuffer;
	delete vertexBuffer;
}

const size_t MeshPrimitive::GetIndicesSize() const
{
	return indicesSize;
}

VkDescriptorImageInfo MeshPrimitive::GetBaseColorDescriptorInfo() const
{
	VkDescriptorImageInfo baseColorInfo{};
	baseColorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	baseColorInfo.imageView = baseColorTexture->GetImageView();
	baseColorInfo.sampler = baseColorTexture->GetSampler();

	return baseColorInfo;
}

VkDescriptorImageInfo MeshPrimitive::GetRoughnessDescriptorInfo() const
{
	VkDescriptorImageInfo roughnessInfo{};
	roughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	roughnessInfo.imageView = roughnessTexture->GetImageView();
	roughnessInfo.sampler = roughnessTexture->GetSampler();

	return roughnessInfo;
}

VkDescriptorImageInfo MeshPrimitive::GetMetallicDescriptorInfo() const
{
	VkDescriptorImageInfo metallicInfo{};
	metallicInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	metallicInfo.imageView = metallicTexture->GetImageView();
	metallicInfo.sampler = metallicTexture->GetSampler();

	return metallicInfo;
}

const std::vector<VkDescriptorSet>& MeshPrimitive::GetMaterialDescriptorSets() const
{
	return materialDescriptorSets;
}

bool MeshPrimitive::GetTransparencyEnabled() const
{
	return transparencyEnabled;
}

void MeshPrimitive::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VulkanBuffer stagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data = stagingBuffer.Map();
	memcpy(data, vertices.data(), (size_t)bufferSize);
	stagingBuffer.Unmap();

	vertexBuffer = new VulkanBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(device, stagingBuffer.Get(), vertexBuffer->Get(), bufferSize);
}

void MeshPrimitive::CreateIndexBuffer(const std::vector<uint16_t>& indices)
{
	VkDevice logicalDevice = device->GetLogical();

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VulkanBuffer stagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data = stagingBuffer.Map();
	memcpy(data, indices.data(), (size_t)bufferSize);
	stagingBuffer.Unmap();

	indexBuffer = new VulkanBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(device, stagingBuffer.Get(), indexBuffer->Get(), bufferSize);

	// Cache indices size for indices draw call
	indicesSize = indices.size();
}

void MeshPrimitive::CreateUniformDescriptorSets(VkDescriptorPool descriptorPool)
{
	VkDevice logicalDevice = device->GetLogical();

	std::vector<VkDescriptorSetLayout> layouts(VulkanConfig::MAX_FRAMES_IN_FLIGHT, materialDescriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	// Allocate a descriptor set for each frame in flight
	materialDescriptorSets.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, materialDescriptorSets.data()) != VK_SUCCESS)
	{
		std::cerr << "Failed to allocate mesh descriptor sets" << std::endl;
		return;
	}

	// Update the descriptor set for each frame in flight
	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
		
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = materialDescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &GetBaseColorDescriptorInfo();

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = materialDescriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &GetRoughnessDescriptorInfo();

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = materialDescriptorSets[i];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &GetMetallicDescriptorInfo();

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}