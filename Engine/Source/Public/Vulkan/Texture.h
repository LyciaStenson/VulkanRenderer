#pragma once

#include <string>
#include <vector>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanImage;

	enum class TextureType
	{
		Unknown,
		BaseColor,
		Normal,
		MetallicRoughness,
		Occlusion
	};
	
	struct ImageData
	{
		std::vector<uint8_t> pixels;
		int width = 0, height = 0, channels = 0;
	};
	
	class VulkanTexture
	{
	public:
		VulkanTexture(VulkanDevice* device, const std::string& path);
		VulkanTexture(VulkanDevice* device, const unsigned char* pixels, int width, int height, bool sRGB = true);
		VulkanTexture(VulkanDevice* device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags);
		~VulkanTexture();

		VkImageView GetImageView() const;
		VkSampler GetSampler() const;

		void TransitionToShaderRead(VkCommandBuffer commandBuffer);
		void TransitionToColor(VkCommandBuffer commandBuffer);

	private:
		VulkanImage* image;
		VkSampler sampler = VK_NULL_HANDLE;

		VulkanDevice* device;

		void CreateTextureImage(const std::string& path);
		void CreateTextureImage(const unsigned char* pixels, int width, int height, bool sRGB);
		void CreateTextureSampler();
	};
}