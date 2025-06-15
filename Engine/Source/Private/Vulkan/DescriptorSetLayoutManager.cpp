#include <Vulkan/DescriptorSetLayoutManager.h>

#include <iostream>
#include <array>

#include <Vulkan/Device.h>

using namespace VulkanRenderer;

VulkanDescriptorSetLayoutManager::VulkanDescriptorSetLayoutManager(VulkanDevice* device)
	: device(device)
{
	CreateGlobalDescriptorSetLayout();
	CreateMeshDescriptorSetLayout();
	CreateMaterialDescriptorSetLayout();
}

VulkanDescriptorSetLayoutManager::~VulkanDescriptorSetLayoutManager()
{
	vkDestroyDescriptorSetLayout(device->GetLogical(), globalDescriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device->GetLogical(), meshDescriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device->GetLogical(), materialDescriptorSetLayout, nullptr);
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutManager::GetGlobalDescriptorSetLayout() const
{
	return globalDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutManager::GetMeshDescriptorSetLayout() const
{
	return meshDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutManager::GetMaterialDescriptorSetLayout() const
{
	return materialDescriptorSetLayout;
}

void VulkanDescriptorSetLayoutManager::CreateGlobalDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding cameraBinding{};
	cameraBinding.binding = 0;
	cameraBinding.descriptorCount = 1;
	cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	cameraBinding.pImmutableSamplers = nullptr;
	cameraBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding pointLightsBinding{};
	pointLightsBinding.binding = 1;
	pointLightsBinding.descriptorCount = 1;
	pointLightsBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	pointLightsBinding.pImmutableSamplers = nullptr;
	pointLightsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {cameraBinding, pointLightsBinding};
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device->GetLogical(), &layoutInfo, nullptr, &globalDescriptorSetLayout) != VK_SUCCESS)
	{
		std::cerr << "Failed to create global descriptor set layout" << std::endl;
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

	//VkDescriptorSetLayoutBinding pointLightsBinding{};
	//pointLightsBinding.binding = 4;
	//pointLightsBinding.descriptorCount = 1;
	//pointLightsBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	//pointLightsBinding.pImmutableSamplers = nullptr;
	//pointLightsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
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