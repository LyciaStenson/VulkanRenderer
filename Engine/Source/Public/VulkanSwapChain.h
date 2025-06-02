#pragma once

#include <volk.h>

#include <VulkanDevice.h>

namespace VulkanRenderer
{
	class VulkanImage;

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VulkanDevice* device, VkSurfaceKHR surface, GLFWwindow* window);
		~VulkanSwapChain();

		VkSwapchainKHR Get() const;

		uint32_t GetImageCount() const;
		uint32_t GetMinImageCount() const;

		void CreateSwapChain();
		void CreateDepthResources();
		void CreateFramebuffers(VkRenderPass renderPass);

		void CleanupSwapChain();

		std::vector<VkFramebuffer> framebuffers;
		VkExtent2D extent;

		VkFormat imageFormat;

		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;

	private:
		VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkSwapchainKHR swapChain = VK_NULL_HANDLE;

		std::vector<VulkanImage*> images;

		VkSurfaceKHR surface;

		VulkanImage* depthImage;

		VulkanDevice* device;

		GLFWwindow* window;

		uint32_t imageCount;
		uint32_t minImageCount;
	};
}