#include <VulkanSwapChain.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <array>

#include <VulkanHelpers.h>
#include <VulkanImage.h>

using namespace VulkanRenderer;

VulkanSwapChain::VulkanSwapChain(VulkanDevice* device, VkSurfaceKHR surface, GLFWwindow* window)
	: device(device), surface(surface), window(window)
{
	CreateSwapChain();
	CreateDepthResources();
}

VulkanSwapChain::~VulkanSwapChain()
{
	CleanupSwapChain();
}

VkSwapchainKHR VulkanSwapChain::Get() const
{
	return swapChain;
}

uint32_t VulkanSwapChain::GetImageCount() const
{
	return imageCount;
}

uint32_t VulkanSwapChain::GetMinImageCount() const
{
	return minImageCount;
}

void VulkanSwapChain::CreateSwapChain()
{
	VkDevice logicalDevice = device->GetLogical();
	VkPhysicalDevice physicalDevice = device->GetPhysical();

	SwapChainSupportDetails supportDetails = QuerySwapChainSupport(physicalDevice, surface);
	
	surfaceFormat = ChooseSwapSurfaceFormat(supportDetails.formats);
	presentMode = ChooseSwapPresentMode(supportDetails.presentModes);
	
	extent = ChooseSwapExtent(supportDetails.capabilities);
	
	minImageCount = supportDetails.capabilities.minImageCount;
	imageCount = minImageCount + 1;
	if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount)
		imageCount = supportDetails.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(device->GetPhysical(), surface);
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = supportDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		std::cerr << "Failed to create swap chain" << std::endl;
		return;
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	
	std::vector<VkImage> rawImages(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, rawImages.data());
	
	images.clear();
	for (VkImage image : rawImages)
	{
		images.push_back(new VulkanImage(device, image, surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT));
	}
	
	imageFormat = surfaceFormat.format;
}

void VulkanSwapChain::CleanupSwapChain()
{
	VkDevice logicalDevice = device->GetLogical();

	delete depthImage;

	for (VkFramebuffer framebuffer : framebuffers)
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);

	for (VulkanImage* image : images)
		delete image;

	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}

void VulkanSwapChain::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat(device->GetPhysical());
	depthImage = new VulkanImage(device, extent.width, extent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	depthImage->TransitionImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void VulkanSwapChain::CreateFramebuffers(VkRenderPass renderPass)
{
	framebuffers.resize(images.size());

	for (size_t i = 0; i < images.size(); i++)
	{
		std::array<VkImageView, 2> attachments =
		{
			images[i]->GetImageView(),
			depthImage->GetImageView()
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device->GetLogical(), &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
		{
			std::cerr << "Failed to create framebuffers" << std::endl;
		}
	}
}

VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

	actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	
	return actualExtent;
}