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
	: device(device), descriptorSetLayout(descriptorSetLayout), name(name), transparencyEnabled(info.enableTransparency)
{
	//baseColorTexture = new VulkanTexture(device, info.baseColorPath);
	//roughnessTexture = new VulkanTexture(device, info.roughnessPath);
	//metallicTexture = new VulkanTexture(device, info.metallicPath);
	CreateVertexBuffer(info.vertices);
	CreateIndexBuffer(info.indices);
}

Mesh::~Mesh()
{
	delete indexBuffer;
	delete vertexBuffer;
	//delete metallicTexture;
	//delete roughnessTexture;
	//delete baseColorTexture;
}

const size_t Mesh::GetIndicesSize() const
{
	return indicesSize;
}

const std::string& Mesh::GetName() const
{
	return name;
}

VkDescriptorImageInfo Mesh::GetBaseColorDescriptorInfo() const
{
	VkDescriptorImageInfo baseColorInfo{};
	baseColorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	baseColorInfo.imageView = baseColorTexture->GetImageView();
	baseColorInfo.sampler = baseColorTexture->GetSampler();

	return baseColorInfo;
}

VkDescriptorImageInfo Mesh::GetRoughnessDescriptorInfo() const
{
	VkDescriptorImageInfo roughnessInfo{};
	roughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	roughnessInfo.imageView = roughnessTexture->GetImageView();
	roughnessInfo.sampler = roughnessTexture->GetSampler();

	return roughnessInfo;
}

VkDescriptorImageInfo Mesh::GetMetallicDescriptorInfo() const
{
	VkDescriptorImageInfo metallicInfo{};
	metallicInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	metallicInfo.imageView = metallicTexture->GetImageView();
	metallicInfo.sampler = metallicTexture->GetSampler();

	return metallicInfo;
}

VkDescriptorSetLayout Mesh::GetDescriptorSetLayout() const
{
	return descriptorSetLayout;
}

bool Mesh::GetTransparencyEnabled() const
{
	return transparencyEnabled;
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