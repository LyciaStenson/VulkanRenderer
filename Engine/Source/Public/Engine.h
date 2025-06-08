#pragma once

#include <unordered_set>
#include <vector>
#include <memory>

#include <volk.h>

#include <Camera.h>
#include <Mesh.h>

namespace VulkanRenderer
{
	class GlfwWindow;
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapChain;
	class VulkanRenderPass;
	class VulkanDescriptorSetLayoutManager;
	class VulkanPipeline;
	class VulkanDescriptorPool;
	class VulkanSync;
	class MeshManager;
	class MeshInstance;
	class Scene;
	class VulkanImGuiOverlay;

	class Engine
	{
	public:
		Engine();
		~Engine();

		void Run();

		bool framebufferResized = false;

	private:
		std::unique_ptr<GlfwWindow> glfwWindow;
		std::unique_ptr<VulkanInstance> instance;
		std::unique_ptr<VulkanDevice> device;
		std::unique_ptr<VulkanSwapChain> swapChain;
		std::unique_ptr<VulkanRenderPass> renderPass;
		std::unique_ptr<VulkanDescriptorSetLayoutManager> descriptorSetLayoutManager;
		std::unique_ptr<VulkanPipeline> opaquePipeline;
		std::unique_ptr<VulkanPipeline> transparentPipeline;
		std::unique_ptr<VulkanDescriptorPool> descriptorPool;
		std::unique_ptr<VulkanSync> sync;

		std::unique_ptr<VulkanImGuiOverlay> imGuiOverlay;
		
		std::unique_ptr<Camera> camera;

		std::unique_ptr<Scene> scene;
		std::unique_ptr<MeshManager> meshManager;
		
		int currentFrame = 0;
		
		void DrawFrame();
		void RecreateSwapChain();
	};
}