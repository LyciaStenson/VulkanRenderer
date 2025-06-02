#pragma once

#include <string>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanImage;

	class VulkanTexture
	{
	public:
		VulkanTexture(VulkanDevice* device, const std::string& path);
		~VulkanTexture();

		VkImageView GetImageView() const;
		VkSampler GetSampler() const;

	private:
		VulkanImage* image;
		VkSampler sampler;

		VulkanDevice* device;

		void CreateTextureImage(const std::string& path);
		void CreateTextureSampler();
	};
}