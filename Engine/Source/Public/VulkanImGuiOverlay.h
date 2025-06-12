#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include <SceneOutliner.h>
#include <Inspector.h>
#include <AssetBrowser.h>

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
		
		void Render(VkCommandBuffer commandBuffer);
		
		//void DrawSceneGraph(std::vector<std::unique_ptr<SceneObject>>& meshInstances);

		//void DrawInspector(bool* show);

		//void DrawAssetBrowser(bool* show);

	private:
		GLFWwindow* glfwWindow;

		std::unique_ptr<ImGuiDescriptorPool> descriptorPool;

		SceneObject* selectedObject = nullptr;

		std::unordered_map<std::string, std::unique_ptr<ImGuiWindow>> m_Windows;

		//SceneOutliner sceneOutliner;
		//Inspector inspector;
		//AssetBrowser assetBrowser;

		void NewFrame();
		void Draw(VkCommandBuffer commandBuffer);

		//void DrawSceneNode(SceneObject* meshInstance);
	};
}