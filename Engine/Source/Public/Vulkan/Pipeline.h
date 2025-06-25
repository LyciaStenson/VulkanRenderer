#pragma once

#include <vector>
#include <memory>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanRenderPass;
	class VulkanDescriptorSetLayoutManager;
	class GlobalDescriptorSetManager;
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
		VulkanPipeline(VulkanDevice* device, VulkanRenderPass* renderPass, VulkanDescriptorSetLayoutManager* descriptorSetLayoutManager, GlobalDescriptorSetManager* globalDescriptorSetManager, PipelineType type);
		~VulkanPipeline();

		void SetDescriptorPool(VkDescriptorPool pool);
		
		void Render(VkCommandBuffer commandBuffer, uint32_t currentFrame, const std::vector<MeshInstance*>& mesheInstances, Camera* camera);

	private:
		void CreateGraphicsPipeline(VulkanDescriptorSetLayoutManager* layoutManager);

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		
		VkDescriptorPool descriptorPool;

		PipelineType type;
		
		VulkanRenderPass* renderPass;

		GlobalDescriptorSetManager* globalDescriptorSetManager;
		
		VulkanDevice* device;
	};
}