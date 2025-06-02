#pragma once

#include <vector>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanSync
	{
	public:
		VulkanSync(VkDevice logicalDevice);
		~VulkanSync();

		void CreateSyncObjects();
		void CleanupSyncObjects();

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

	private:
		VkDevice logicalDevice;
	};
}