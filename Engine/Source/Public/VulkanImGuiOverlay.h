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
	class SceneObject;
	
	class VulkanImGuiOverlay
	{
	public:
		VulkanImGuiOverlay(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow);
		~VulkanImGuiOverlay();

		void NewFrame();
		void Draw(VkCommandBuffer commandBuffer);

		void DrawSceneGraph(std::vector<std::unique_ptr<SceneObject>>& meshInstances);
		void DrawSceneNode(SceneObject* meshInstance);

		void DrawInspector();

	private:
		GLFWwindow* glfwWindow;

		std::unique_ptr<ImGuiDescriptorPool> descriptorPool;

		SceneObject* selectedObject = nullptr;
	};
}