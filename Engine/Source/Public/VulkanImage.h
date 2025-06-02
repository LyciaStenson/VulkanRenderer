#pragma once

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;

	class VulkanImage
	{
	public:
		VulkanImage(VulkanDevice* device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImageAspectFlags aspectFlags);
		VulkanImage(VulkanDevice* device, VkImage existingImage, VkFormat format, VkImageAspectFlags aspectFlags);
		~VulkanImage();

		VkImage Get() const;
		VkImageView GetImageView() const;

		void CreateImageView(VkImageAspectFlags aspectFlags);

		void TransitionImageLayout(VkImageLayout newLayout);

	private:
		VkImage image;
		VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkFormat format;

		VkImageView imageView;
		VkDeviceMemory memory;

		VulkanDevice* device;

		bool ownsImage;

		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage& image, VkDeviceMemory& imageMemory);
	};
}