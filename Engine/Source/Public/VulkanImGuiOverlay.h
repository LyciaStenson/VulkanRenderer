#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include <ImGuiWindow.h>

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
	class Scene;
	class SceneObject;
	
	class VulkanImGuiOverlay
	{
	public:
		VulkanImGuiOverlay(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene);
		~VulkanImGuiOverlay();

		SceneObject* GetSelectedObject() const;
		void SelectObject(SceneObject* object);
		
		void Render(VkCommandBuffer commandBuffer);

		void OpenCreateObjectWindow();

	private:
		GLFWwindow* m_Window;

		std::unique_ptr<ImGuiDescriptorPool> m_DescriptorPool;

		SceneObject* m_SelectedObject = nullptr;

		std::unordered_map<std::string, std::unique_ptr<ImGuiWindow>> m_Windows;
		
		void NewFrame();
		void Draw(VkCommandBuffer commandBuffer);
	};
}