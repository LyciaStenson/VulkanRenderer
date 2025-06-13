#include <Vulkan/DescriptorPool.h>

#include <iostream>
#include <array>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>

using namespace VulkanRenderer;

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* device, size_t meshCount)
	: device(device)
{
	CreateDescriptorPool(meshCount);
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(device->GetLogical(), descriptorPool, nullptr);
}

VkDescriptorPool VulkanDescriptorPool::Get() const
{
	return descriptorPool;
}

void VulkanDescriptorPool::CreateDescriptorPool(size_t meshCount)
{
	constexpr uint32_t SET_COUNT = 2;
	constexpr uint32_t SAMPLER_COUNT = 2;

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(meshCount * VulkanConfig::MAX_FRAMES_IN_FLIGHT);

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(meshCount * VulkanConfig::MAX_FRAMES_IN_FLIGHT * SAMPLER_COUNT * SET_COUNT);
	
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(meshCount * VulkanConfig::MAX_FRAMES_IN_FLIGHT * SET_COUNT);

	if (vkCreateDescriptorPool(device->GetLogical(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		std::cerr << "Failed to create descriptor pool" << std::endl;
	}
}