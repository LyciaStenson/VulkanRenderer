#pragma once

#include <vector>
#include <memory>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanSwapChain;
	class VulkanRenderPass;
	class Mesh;
	class Camera;
	class VulkanImGuiOverlay;

	class VulkanPipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass);
		~VulkanPipeline();

		void SetDescriptorPool(VkDescriptorPool pool);
		void SetImGuiOverlay(VulkanImGuiOverlay* overlay);

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame, std::vector<std::unique_ptr<Mesh>>& meshes, std::vector<std::unique_ptr<Mesh>>& transparentMeshes, Camera* camera);

		VkDescriptorSetLayout GetCameraDescriptorSetLayout() const;
		VkDescriptorSetLayout GetMeshDescriptorSetLayout() const;

	private:
		void CreateCameraDescriptorSetLayout();
		void CreateMeshDescriptorSetLayout();
		void CreateGraphicsPipeline();

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;

		VkDescriptorSetLayout cameraDescriptorSetLayout;
		VkDescriptorSetLayout meshDescriptorSetLayout;

		VkDescriptorPool descriptorPool;

		VulkanSwapChain* swapChain;
		VulkanRenderPass* renderPass;

		VulkanImGuiOverlay* imGuiOverlay;

		VulkanDevice* device;
	};
}