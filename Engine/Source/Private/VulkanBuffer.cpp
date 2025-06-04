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
	{
		VmaAllocator allocator = device->GetAllocator();
		vmaDestroyBuffer(allocator, buffer, allocation);
		//vkDestroyBuffer(device->GetLogical(), buffer, nullptr);
	}
	//if (memory != VK_NULL_HANDLE)
		//vkFreeMemory(device->GetLogical(), memory, nullptr);
}

VkBuffer VulkanBuffer::Get() const
{
	return buffer;
}

VmaAllocation VulkanBuffer::GetAllocation() const
{
	return allocation;
}

void* VulkanBuffer::Map()
{
	void* mappedData;

	if (vmaMapMemory(device->GetAllocator(), allocation, &mappedData) != VK_SUCCESS)
	{
		std::cerr << "Faile to map VulkanBuffer memory" << std::endl;
	}

	return mappedData;
}

void VulkanBuffer::Unmap()
{
	vmaUnmapMemory(device->GetAllocator(), allocation);
}

void VulkanBuffer::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
{
	VmaAllocator allocator = device->GetAllocator();

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocationInfo{};
	allocationInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;

	if (propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		if (propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			allocationInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		else
			allocationInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	}
	else
	{
		allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	}

	if (vmaCreateBuffer(allocator, &bufferInfo, &allocationInfo, &buffer, &allocation, nullptr) != VK_SUCCESS)
	{
		std::cerr << "Failed to create buffer" << std::endl;
	}
}