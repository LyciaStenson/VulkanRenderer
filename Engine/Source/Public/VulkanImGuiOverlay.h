#pragma once

#include <vector>
#include <memory>

#include <volk.h>
#include <glfw/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace VulkanRenderer
{
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapChain;
	class VulkanRenderPass;
	class ImGuiDescriptorPool;
	
	class VulkanImGuiOverlay
	{
	public:
		VulkanImGuiOverlay(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow);
		~VulkanImGuiOverlay();

		void NewFrame();
		void Draw(VkCommandBuffer commandBuffer);

	private:
		GLFWwindow* glfwWindow;

		std::unique_ptr<ImGuiDescriptorPool> descriptorPool;
	};
}