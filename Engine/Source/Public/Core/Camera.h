#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <Core/SceneObject.h>
#include <Core/Transform.h>
#include <Vulkan/UniformBuffer.h>

namespace VulkanRenderer
{
	class VulkanDevice;

	class Camera : public SceneObject
	{
	public:
		Camera(const std::string& name, VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);
		~Camera();

		void CreateDescriptorSets(VkDescriptorPool descriptorPool);

		void UpdateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent);
		
		float fov = 70.0f;

		std::vector<VkDescriptorSet> descriptorSets;

	private:
		VulkanDevice* device;

		VkDescriptorSetLayout descriptorSetLayout;
		
		std::vector<VulkanUniformBuffer> uniformBuffers;
		
		void CreateUniformBuffers();
	};
}