#include <Vulkan/Sync.h>

#include <iostream>

#include <Vulkan/Config.h>

using namespace VulkanRenderer;

VulkanSync::VulkanSync(VkDevice logicalDevice)
	: logicalDevice(logicalDevice)
{
	CreateSyncObjects();
}

VulkanSync::~VulkanSync()
{
	CleanupSyncObjects();
}

void VulkanSync::CreateSyncObjects()
{
	imageAvailableSemaphores.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (
			vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS
			)
		{
			std::cerr << "Failed to create semaphores and fence" << std::endl;
			return;
		}
	}
}

void VulkanSync::CleanupSyncObjects()
{
	for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
	}

	renderFinishedSemaphores.clear();
	imageAvailableSemaphores.clear();
	inFlightFences.clear();
}