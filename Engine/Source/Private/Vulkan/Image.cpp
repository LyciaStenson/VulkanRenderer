#include <Vulkan/Image.h>

#include <iostream>

#include <Vulkan/Helpers.h>
#include <Vulkan/Device.h>

using namespace VulkanRenderer;

VulkanImage::VulkanImage(VulkanDevice* device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImageAspectFlags aspectFlags)
	: device(device), format(format), ownsImage(true)
{
	CreateImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, usageFlags, propertyFlags, image, memory);
	CreateImageView(aspectFlags);
}

VulkanImage::VulkanImage(VulkanDevice* device, VkImage existingImage, VkFormat format, VkImageAspectFlags aspectFlags)
	: device(device), format(format), image(existingImage), ownsImage(false)
{
	CreateImageView(aspectFlags);
}

VulkanImage::~VulkanImage()
{
	VkDevice logicalDevice = device->GetLogical();
	
	if (imageView != VK_NULL_HANDLE)
		vkDestroyImageView(logicalDevice, imageView, nullptr);

	if (ownsImage)
	{
		if (image != VK_NULL_HANDLE)
			vkDestroyImage(logicalDevice, image, nullptr);
		if (memory != VK_NULL_HANDLE)
			vkFreeMemory(logicalDevice, memory, nullptr);
	}
}

VkImage VulkanImage::Get() const
{
	return image;
}

VkImageView VulkanImage::GetImageView() const
{
	return imageView;
}

void VulkanImage::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkDevice logicalDevice = device->GetLogical();

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usageFlags;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		std::cerr << "Failed to create image" << std::endl;
		return;
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(logicalDevice, image, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = device->FindMemoryType(memoryRequirements.memoryTypeBits, propertyFlags);

	if (vkAllocateMemory(logicalDevice, &allocateInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		std::cerr << "Failed to allocate image memory" << std::endl;
		return;
	}

	vkBindImageMemory(logicalDevice, image, imageMemory, 0);
}

void VulkanImage::CreateImageView(VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;

	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	
	if (vkCreateImageView(device->GetLogical(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		std::cerr << "Failed to create image view" << std::endl;
}

void VulkanImage::TransitionImageLayout(VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = device->BeginSingleTimeCommands();

	VkImageMemoryBarrier memoryBarrier{};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.oldLayout = currentLayout;
	memoryBarrier.newLayout = newLayout;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = image;
	memoryBarrier.subresourceRange.baseMipLevel = 0;
	memoryBarrier.subresourceRange.levelCount = 1;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;

	memoryBarrier.srcAccessMask = 0;
	memoryBarrier.dstAccessMask = 0;

	VkPipelineStageFlags sourceStageFlags;
	VkPipelineStageFlags destinationStageFlags;
	
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(format))
		{
			memoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		memoryBarrier.srcAccessMask = 0;
		memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		memoryBarrier.srcAccessMask = 0;
		memoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		std::cerr << "Unsupported layout transition" << std::endl;
		return;
	}

	currentLayout = newLayout;

	vkCmdPipelineBarrier
	(
		commandBuffer,
		sourceStageFlags, destinationStageFlags,
		0,
		0, nullptr,
		0, nullptr,
		1, &memoryBarrier
	);

	device->EndSingleTimeCommands(commandBuffer);
}