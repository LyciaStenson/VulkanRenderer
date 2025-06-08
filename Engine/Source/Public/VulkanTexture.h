#pragma once

#include <string>

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

	class VulkanTexture
	{
	public:
		VulkanTexture(VulkanDevice* device, const std::string& path);
		VulkanTexture(VulkanDevice* device, const unsigned char* pixels, int width, int height, bool sRGB = true);
		~VulkanTexture();

		VkImageView GetImageView() const;
		VkSampler GetSampler() const;

	private:
		VulkanImage* image;
		VkSampler sampler;

		VulkanDevice* device;

		void CreateTextureImage(const std::string& path);
		void CreateTextureImage(const unsigned char* pixels, int width, int height, bool sRGB);
		void CreateTextureSampler();
	};
}