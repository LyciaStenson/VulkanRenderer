#include <Vulkan/StorageBuffer.h>

#include <Vulkan/Buffer.h>
#include <Vulkan/Device.h>

#include <iostream>

using namespace VulkanRenderer;

VulkanStorageBuffer::VulkanStorageBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
	: device(device)
{
	buffer = new VulkanRenderer::VulkanBuffer(device, size, usageFlags | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, propertyFlags);

	if (propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		buffer->Map(mappedData);
}

VulkanStorageBuffer::~VulkanStorageBuffer()
{
	if (mappedData)
		vmaUnmapMemory(device->GetAllocator(), buffer->GetAllocation());
	
	delete buffer;
}

VkBuffer VulkanStorageBuffer::Get() const
{
	return buffer->Get();
}

void* VulkanStorageBuffer::GetMappedData() const
{
	return mappedData;
}

void VulkanStorageBuffer::UploadData(const void* data, VkDeviceSize size)
{
	if (!mappedData)
		return;
	
	std::memcpy(mappedData, data, static_cast<size_t>(size));
}