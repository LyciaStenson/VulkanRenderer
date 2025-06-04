#include <VulkanUniformBuffer.h>

#include <iostream>

#include <VulkanDevice.h>

using namespace VulkanRenderer;

VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
	: device(device)
{
	buffer = new VulkanBuffer(device, size, usageFlags, propertyFlags);
	AllocateMemory(size);
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
	if (mappedData)
		vmaUnmapMemory(device->GetAllocator(), buffer->GetAllocation());
	
	delete buffer;
}

VkBuffer VulkanUniformBuffer::Get() const
{
	return buffer->Get();
}

void* VulkanUniformBuffer::GetMappedData() const
{
	return mappedData;
}

void VulkanUniformBuffer::AllocateMemory(VkDeviceSize size)
{
	if (mappedData == nullptr)
	{
		if (vmaMapMemory(device->GetAllocator(), buffer->GetAllocation(), &mappedData) != VK_SUCCESS)
		{
			std::cerr << "Failed to map uniform buffer memory" << std::endl;
		}
	}
}