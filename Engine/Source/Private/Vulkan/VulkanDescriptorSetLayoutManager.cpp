#include <Vulkan/VulkanDescriptorSetLayoutManager.h>

#include <iostream>
#include <array>

#include <Vulkan/VulkanDevice.h>

using namespace VulkanRenderer;

VulkanDescriptorSetLayoutManager::VulkanDescriptorSetLayoutManager(VulkanDevice* device)
	: device(device)
{
	CreateCameraDescriptorSetLayout();
	CreateMeshDescriptorSetLayout();
	CreateMaterialDescriptorSetLayout();
}

VulkanDescriptorSetLayoutManager::~VulkanDescriptorSetLayoutManager()
{
	vkDestroyDescriptorSetLayout(device->GetLogical(), cameraDescriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device->GetLogical(), meshDescriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device->GetLogical(), materialDescriptorSetLayout, nullptr);
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutManager::GetCameraDescriptorSetLayout() const
{
	return cameraDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutManager::GetMeshDescriptorSetLayout() const
{
	return meshDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutManager::GetMaterialDescriptorSetLayout() const
{
	return materialDescriptorSetLayout;
}

void VulkanDescriptorSetLayoutManager::CreateCameraDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboBinding{};
	uboBinding.binding = 0;
	uboBinding.descriptorCount = 1;
	uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboBinding.pImmutableSamplers = nullptr;
	uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboBinding;

	if (vkCreateDescriptorSetLayout(device->GetLogical(), &layoutInfo, nullptr, &cameraDescriptorSetLayout) != VK_SUCCESS)
	{
		std::cerr << "Failed to create camera descriptor set layout" << std::endl;
	}
}

void VulkanDescriptorSetLayoutManager::CreateMeshDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboBinding{};
	uboBinding.binding = 0;
	uboBinding.descriptorCount = 1;
	uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboBinding.pImmutableSamplers = nullptr;
	uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboBinding;

	if (vkCreateDescriptorSetLayout(device->GetLogical(), &layoutInfo, nullptr, &meshDescriptorSetLayout) != VK_SUCCESS)
	{
		std::cerr << "Failed to create mesh descriptor set layout" << std::endl;
	}
}

void VulkanDescriptorSetLayoutManager::CreateMaterialDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding factorsBinding{};
	factorsBinding.binding = 0;
	factorsBinding.descriptorCount = 1;
	factorsBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	factorsBinding.pImmutableSamplers = nullptr;
	factorsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding baseColorBinding{};
	baseColorBinding.binding = 1;
	baseColorBinding.descriptorCount = 1;
	baseColorBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	baseColorBinding.pImmutableSamplers = nullptr;
	baseColorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding metallicRoughnessBinding{};
	metallicRoughnessBinding.binding = 2;
	metallicRoughnessBinding.descriptorCount = 1;
	metallicRoughnessBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	metallicRoughnessBinding.pImmutableSamplers = nullptr;
	metallicRoughnessBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding normalBinding{};
	normalBinding.binding = 3;
	normalBinding.descriptorCount = 1;
	normalBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalBinding.pImmutableSamplers = nullptr;
	normalBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	std::array<VkDescriptorSetLayoutBinding, 4> bindings = {factorsBinding, baseColorBinding, metallicRoughnessBinding, normalBinding};
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device->GetLogical(), &layoutInfo, nullptr, &materialDescriptorSetLayout) != VK_SUCCESS)
	{
		std::cerr << "Failed to create mesh descriptor set layout" << std::endl;
	}
}