#include <Mesh.h>

#include <iostream>
#include <chrono>

#include <stb_image.h>

#include <VulkanDevice.h>
#include <VulkanTexture.h>
#include <MeshPrimitive.h>

using namespace VulkanRenderer;

Mesh::Mesh(VulkanDevice* device, VkDescriptorSetLayout uniformDescriptorSetLayout)
	: device(device), uniformDescriptorSetLayout(uniformDescriptorSetLayout)
{

}

Mesh::~Mesh()
{

}

size_t Mesh::GetPrimitiveCount() const
{
	return primitives.size();
}

MeshPrimitive* Mesh::GetPrimitive(size_t index) const
{
	if (primitives[index])
		return primitives[index].get();
	return nullptr;
}

VkDescriptorSetLayout Mesh::GetUniformDescriptorSetLayout() const
{
	return uniformDescriptorSetLayout;
}

void Mesh::AddPrimitive(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, const MeshPrimitiveInfo& info)
{
	primitives.push_back(std::make_unique<MeshPrimitive>(device, descriptorSetLayout, info));
}