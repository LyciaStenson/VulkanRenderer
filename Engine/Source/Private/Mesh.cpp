#include <Mesh.h>

#include <iostream>
#include <chrono>

#include <stb_image.h>

#include <VulkanConfig.h>
#include <VulkanDevice.h>
#include <VulkanTexture.h>
#include <VulkanBuffer.h>
#include <MeshUBO.h>

using namespace VulkanRenderer;

Mesh::Mesh(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, const std::string& name, const MeshInfo& info)
	: device(device), descriptorSetLayout(descriptorSetLayout), name(name)
{
	baseColorTexture = new VulkanTexture(device, info.baseColorPath);
	roughnessTexture = new VulkanTexture(device, info.roughnessPath);
	metallicTexture = new VulkanTexture(device, info.metallicPath);
	CreateVertexBuffer(info.vertices);
	CreateIndexBuffer(info.indices);
	CreateUniformBuffers();
}

Mesh::~Mesh()
{
	delete indexBuffer;
	delete vertexBuffer;
	delete metallicTexture;
	delete roughnessTexture;
	delete baseColorTexture;
}

size_t Mesh::GetIndicesSize() const
{
	return indicesSize;
}

const std::string& Mesh::GetName() const
{
	return name;
}

void Mesh::CreateDescriptorSets(VkDescriptorPool descriptorPool)
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
		bufferInfo.range = sizeof(MeshUBO);

		VkDescriptorImageInfo baseColorInfo{};
		baseColorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		baseColorInfo.imageView = baseColorTexture->GetImageView();
		baseColorInfo.sampler = baseColorTexture->GetSampler();

		VkDescriptorImageInfo roughnessInfo{};
		roughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		roughnessInfo.imageView = roughnessTexture->GetImageView();
		roughnessInfo.sampler = roughnessTexture->GetSampler();

		VkDescriptorImageInfo metallicInfo{};
		metallicInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		metallicInfo.imageView = metallicTexture->GetImageView();
		metallicInfo.sampler = metallicTexture->GetSampler();

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
		descriptorWrites[1].pImageInfo = &baseColorInfo;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = descriptorSets[i];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &roughnessInfo;

		descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[3].dstSet = descriptorSets[i];
		descriptorWrites[3].dstBinding = 3;
		descriptorWrites[3].dstArrayElement = 0;
		descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[3].descriptorCount = 1;
		descriptorWrites[3].pImageInfo = &metallicInfo;

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void Mesh::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VulkanBuffer stagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data = stagingBuffer.Map();
	memcpy(data, vertices.data(), (size_t)bufferSize);
	stagingBuffer.Unmap();

	vertexBuffer = new VulkanBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(device, stagingBuffer.Get(), vertexBuffer->Get(), bufferSize);
}

void Mesh::CreateIndexBuffer(const std::vector<uint16_t>& indices)
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

void Mesh::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(MeshUBO);
	uniformBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		uniformBuffers.emplace_back(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void Mesh::UpdateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent)
{
	MeshUBO ubo{};
	ubo.model = glm::translate(glm::mat4(1.0f), transform.position) * glm::mat4_cast(transform.rotation) * glm::scale(glm::mat4(1.0f), transform.scale);

	memcpy(uniformBuffers[currentImage].GetMappedData(), &ubo, sizeof(ubo));
}