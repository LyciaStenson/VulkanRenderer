#include <VulkanDescriptorPool.h>

#include <iostream>
#include <array>

#include <VulkanConfig.h>
#include <VulkanDevice.h>

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
	std::array<VkDescriptorPoolSize, 4> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(meshCount * VulkanConfig::MAX_FRAMES_IN_FLIGHT);

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(meshCount * VulkanConfig::MAX_FRAMES_IN_FLIGHT) * 2;

	poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[2].descriptorCount = static_cast<uint32_t>(meshCount * VulkanConfig::MAX_FRAMES_IN_FLIGHT) * 2;

	poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[3].descriptorCount = static_cast<uint32_t>(meshCount * VulkanConfig::MAX_FRAMES_IN_FLIGHT) * 2;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(meshCount * VulkanConfig::MAX_FRAMES_IN_FLIGHT * 2);

	if (vkCreateDescriptorPool(device->GetLogical(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		std::cerr << "Failed to create descriptor pool" << std::endl;
	}
}