#pragma once

#include <vector>
#include <memory>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanRenderPass;
	class VulkanDescriptorSetLayoutManager;
	class Mesh;
	class Camera;
	class VulkanImGuiOverlay;

	enum class PipelineType
	{
		Opaque,
		Transparent
	};

	class VulkanPipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, VulkanRenderPass* renderPass, VulkanDescriptorSetLayoutManager* layoutManager, PipelineType type);
		~VulkanPipeline();

		void SetDescriptorPool(VkDescriptorPool pool);
		void SetImGuiOverlay(VulkanImGuiOverlay* overlay);

		void Render(VkCommandBuffer commandBuffer, uint32_t currentFrame, std::vector<std::unique_ptr<Mesh>>& meshes, Camera* camera);

	private:
		void CreateGraphicsPipeline(VulkanDescriptorSetLayoutManager* layoutManager);

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		
		VkDescriptorPool descriptorPool;

		PipelineType type;
		
		VulkanRenderPass* renderPass;

		VulkanImGuiOverlay* imGuiOverlay;

		VulkanDevice* device;
	};
}