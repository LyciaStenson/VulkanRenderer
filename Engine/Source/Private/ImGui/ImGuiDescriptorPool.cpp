#include <ImGui/ImGuiDescriptorPool.h>

#include <iostream>

#include <imgui.h>

#include <Vulkan/VulkanDevice.h>

using namespace VulkanRenderer;

ImGuiDescriptorPool::ImGuiDescriptorPool(VulkanDevice* device)
	: device(device)
{
	CreateDescriptorPool();
}

ImGuiDescriptorPool::~ImGuiDescriptorPool()
{
	vkDestroyDescriptorPool(device->GetLogical(), descriptorPool, nullptr);
}

VkDescriptorPool ImGuiDescriptorPool::Get() const
{
	return descriptorPool;
}

void ImGuiDescriptorPool::CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSizes[] =
	{
		{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
	};

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
	poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
	poolInfo.pPoolSizes = poolSizes;

	if (vkCreateDescriptorPool(device->GetLogical(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		std::cerr << "Failed to create descriptor pool" << std::endl;
	}
}