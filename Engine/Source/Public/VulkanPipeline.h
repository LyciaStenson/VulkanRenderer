#pragma once

#include <vector>
#include <memory>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanRenderPass;
	class VulkanDescriptorSetLayoutManager;
	class MeshInstance;
	class Mesh;
	class Camera;
	
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
		
		void Render(VkCommandBuffer commandBuffer, uint32_t currentFrame, const std::vector<std::unique_ptr<MeshInstance>>& mesheInstances, Camera* camera);

	private:
		void CreateGraphicsPipeline(VulkanDescriptorSetLayoutManager* layoutManager);

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		
		VkDescriptorPool descriptorPool;

		PipelineType type;
		
		VulkanRenderPass* renderPass;
		
		VulkanDevice* device;
	};
}