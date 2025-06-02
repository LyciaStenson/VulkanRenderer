#include <VulkanUniformBuffer.h>

#include <iostream>

#include <VulkanDevice.h>

using namespace VulkanRenderer;

VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
	: device(device)
{
	buffer = new VulkanBuffer(device, size, usageFlags, propertyFlags);
	AllocateMemory(size, propertyFlags);
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
	if (mappedData)
		vkUnmapMemory(device->GetLogical(), buffer->GetMemory());

	delete buffer;
}

VkBuffer VulkanUniformBuffer::Get() const
{
	return buffer->Get();
}

VkDeviceMemory VulkanUniformBuffer::GetMemory() const
{
	return buffer->GetMemory();
}

void* VulkanUniformBuffer::GetMappedData() const
{
	return mappedData;
}

void VulkanUniformBuffer::AllocateMemory(VkDeviceSize size, VkMemoryPropertyFlags propertyFlags)
{
	if (mappedData == nullptr)
	{
		if (vkMapMemory(device->GetLogical(), buffer->GetMemory(), 0, size, 0, &mappedData))
		{
			std::cerr << "Failed to map uniform buffer memory" << std::endl;
		}
	}
}