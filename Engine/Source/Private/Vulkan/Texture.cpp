#include <Vulkan/Texture.h>

#include <iostream>

#include <Vulkan/Device.h>
#include <Vulkan/Image.h>
#include <Vulkan/Buffer.h>

#include <stb_image.h>

using namespace VulkanRenderer;

VulkanTexture::VulkanTexture(VulkanDevice* device, const std::string& path)
	: device(device)
{
	CreateTextureImage(path);
	CreateTextureSampler();
}

VulkanTexture::VulkanTexture(VulkanDevice* device, const unsigned char* pixels, int width, int height, bool sRGB)
	: device(device)
{
	CreateTextureImage(pixels, width, height, sRGB);
	CreateTextureSampler();
}

VulkanTexture::~VulkanTexture()
{
	vkDestroySampler(device->GetLogical(), sampler, nullptr);
	delete image;
}

VkImageView VulkanTexture::GetImageView() const
{
	return image->GetImageView();
}

VkSampler VulkanTexture::GetSampler() const
{
	return sampler;
}

void VulkanTexture::CreateTextureImage(const std::string& path)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	VkDeviceSize imageSize = width * height * 4;

	if (!pixels)
	{
		std::cerr << "Failed to load texture image" << std::endl;
		return;
	}

	VulkanBuffer stagingBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data = stagingBuffer.Map();
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	stagingBuffer.Unmap();

	stbi_image_free(pixels);

	image = new VulkanImage(device, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	
	image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(device, stagingBuffer.Get(), image->Get(), static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	image->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void VulkanTexture::CreateTextureImage(const unsigned char* pixels, int width, int height, bool sRGB)
{
	VkDeviceSize imageSize = width * height * 4;

	VulkanBuffer stagingBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data = stagingBuffer.Map();
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	stagingBuffer.Unmap();

	VkFormat format = sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;

	image = new VulkanImage(device, width, height, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(device, stagingBuffer.Get(), image->Get(), static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	image->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void VulkanTexture::CreateTextureSampler()
{
	VkPhysicalDeviceProperties deviceProperties{};
	vkGetPhysicalDeviceProperties(device->GetPhysical(), &deviceProperties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device->GetLogical(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
	{
		std::cerr << "Failed to create texture sampler" << std::endl;
		return;
	}
}