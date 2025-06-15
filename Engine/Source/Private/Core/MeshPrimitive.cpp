#include <Core/MeshPrimitive.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Helpers.h>
#include <Vulkan/Device.h>
#include <Vulkan/Texture.h>
#include <Vulkan/Buffer.h>
#include <Vulkan/UniformBuffer.h>
#include <Core/MaterialFactorsUBO.h>
#include <Core/Vertex.h>

using namespace VulkanRenderer;

MeshPrimitive::MeshPrimitive(VulkanDevice* device, VkDescriptorSetLayout materialDescriptorSetLayout, VkDescriptorPool descriptorPool, const MeshPrimitiveInfo& info)
	:
	device(device),
	materialDescriptorSetLayout(materialDescriptorSetLayout),
	baseColorFactor(info.baseColorFactor),
	metallicFactor(info.metallicFactor),
	roughnessFactor(info.roughnessFactor),
	baseColorTexture(info.baseColorTexture),
	metallicRoughnessTexture(info.metallicRoughnessTexture),
	normalTexture(info.normalTexture),
	transparencyEnabled(info.enableTransparency)
{
	CreateVertexBuffer(info.vertices);
	CreateIndexBuffer(info.indices);
	CreateMaterialFactorsUniformBuffer();
	CreateMaterialDescriptorSets(descriptorPool);
}

MeshPrimitive::~MeshPrimitive()
{
	delete materialFactorsUniformBuffer;
	delete indexBuffer;
	delete vertexBuffer;
}

const size_t MeshPrimitive::GetIndicesSize() const
{
	return indicesSize;
}

VkDescriptorImageInfo MeshPrimitive::GetBaseColorInfo() const
{
	VkDescriptorImageInfo baseColorInfo{};
	baseColorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	baseColorInfo.imageView = baseColorTexture->GetImageView();
	baseColorInfo.sampler = baseColorTexture->GetSampler();

	return baseColorInfo;
}

VkDescriptorImageInfo MeshPrimitive::GetMetallicRoughnessInfo() const
{
	VkDescriptorImageInfo roughnessInfo{};
	roughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	roughnessInfo.imageView = metallicRoughnessTexture->GetImageView();
	roughnessInfo.sampler = metallicRoughnessTexture->GetSampler();

	return roughnessInfo;
}

VkDescriptorImageInfo MeshPrimitive::GetNormalInfo() const
{
	VkDescriptorImageInfo metallicInfo{};
	metallicInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	metallicInfo.imageView = normalTexture->GetImageView();
	metallicInfo.sampler = normalTexture->GetSampler();

	return metallicInfo;
}

VkDescriptorBufferInfo MeshPrimitive::GetPointLightBufferInfo() const
{
	VkDescriptorBufferInfo bufferInfo{};
	//bufferInfo.buffer = pointLightBuffer;
	bufferInfo.offset = 0;
	//bufferInfo.range = pointLightBufferSize;

	return bufferInfo;
}

const std::vector<VkDescriptorSet>& MeshPrimitive::GetMaterialDescriptorSets() const
{
	return materialDescriptorSets;
}

bool MeshPrimitive::GetTransparencyEnabled() const
{
	return transparencyEnabled;
}

void MeshPrimitive::CreateMaterialFactorsUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(MaterialFactorsUBO);
	
	materialFactorsUniformBuffer = new VulkanUniformBuffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	MaterialFactorsUBO ubo{};
	ubo.baseColor = baseColorFactor;
	ubo.metallicRoughness = glm::vec3(0.0f, roughnessFactor, metallicFactor);
	
	memcpy(materialFactorsUniformBuffer->GetMappedData(), &ubo, sizeof(ubo));
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

void MeshPrimitive::CreateMaterialDescriptorSets(VkDescriptorPool descriptorPool)
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
		std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
		
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = materialFactorsUniformBuffer->Get();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(MaterialFactorsUBO);

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = materialDescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = materialDescriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &GetBaseColorInfo();

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = materialDescriptorSets[i];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &GetMetallicRoughnessInfo();

		descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[3].dstSet = materialDescriptorSets[i];
		descriptorWrites[3].dstBinding = 3;
		descriptorWrites[3].dstArrayElement = 0;
		descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[3].descriptorCount = 1;
		descriptorWrites[3].pImageInfo = &GetNormalInfo();

		//descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrites[4].dstSet = materialDescriptorSets[i];
		//descriptorWrites[4].dstBinding = 4;
		//descriptorWrites[4].dstArrayElement = 0;
		//descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		//descriptorWrites[4].descriptorCount = 1;
		//descriptorWrites[4].pBufferInfo = &GetPointLightBufferInfo();

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}