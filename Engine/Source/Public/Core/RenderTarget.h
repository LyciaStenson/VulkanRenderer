#pragma once

#include <memory>

#include <Vulkan/RenderPass.h>
#include <Vulkan/Pipeline.h>

#include <volk.h>

namespace VulkanRenderer
{
	class Renderer;
	class Scene;
	class VulkanDescriptorSetLayoutManager;
	class GlobalDescriptorSetManager;

	class RenderTarget
	{
	public:
		RenderTarget(Renderer* renderer);
		virtual ~RenderTarget() = default;
		
		virtual void Render(Scene* scene, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) = 0;

	protected:
		Renderer* renderer = nullptr;
	};
}