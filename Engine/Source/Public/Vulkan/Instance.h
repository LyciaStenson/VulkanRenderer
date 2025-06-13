#pragma once

#include <vector>

#include <GLFW/glfw3.h>
#include <volk.h>

namespace VulkanRenderer
{
	class VulkanInstance
	{
	public:
		VulkanInstance(GLFWwindow* window);
		~VulkanInstance();

		VkInstance Get() const;
		VkSurfaceKHR GetSurface() const;

	private:
		VkInstance instance = VK_NULL_HANDLE;

		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

		void CreateInstance();
		void CreateSurface(GLFWwindow* window);

		std::vector<const char*> GetRequiredExtensions();

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void SetupDebugMessenger();

		VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		void DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
	};
}