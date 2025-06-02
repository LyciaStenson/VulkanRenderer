#include <VulkanBuffer.h>

#include <iostream>

#include <VulkanHelpers.h>
#include <VulkanDevice.h>

using namespace VulkanRenderer;

VulkanBuffer::VulkanBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
	: device(device), size(size)
{
	CreateBuffer(usageFlags, propertyFlags);
}

VulkanBuffer::~VulkanBuffer()
{
	if (buffer != VK_NULL_HANDLE)
		vkDestroyBuffer(device->GetLogical(), buffer, nullptr);
	if (memory != VK_NULL_HANDLE)
		vkFreeMemory(device->GetLogical(), memory, nullptr);
}

VkBuffer VulkanBuffer::Get() const
{
	return buffer;
}

VkDeviceMemory VulkanBuffer::GetMemory() const
{
	return memory;
}

void VulkanBuffer::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
{
	VkDevice logicalDevice = device->GetLogical();

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		std::cerr << "Failed to create buffer" << std::endl;
		return;
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = device->FindMemoryType(memoryRequirements.memoryTypeBits, propertyFlags);

	if (vkAllocateMemory(logicalDevice, &allocateInfo, nullptr, &memory) != VK_SUCCESS)
	{
		std::cerr << "Failed to allocate buffer memory" << std::endl;
		return;
	}

	vkBindBufferMemory(logicalDevice, buffer, memory, 0);
}