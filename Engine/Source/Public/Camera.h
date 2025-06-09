#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <SceneObject.h>
#include <Transform.h>
#include <VulkanUniformBuffer.h>

namespace VulkanRenderer
{
	class VulkanDevice;

	class Camera : public SceneObject
	{
	public:
		Camera(const std::string& name, VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout);
		~Camera();

		void CreateDescriptorSets(VkDescriptorPool descriptorPool);

		void UpdateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent);

		std::vector<VkDescriptorSet> descriptorSets;
		
		float fov = 70.0f;

	private:
		VulkanDevice* device;
		
		std::vector<VulkanUniformBuffer> uniformBuffers;

		VkDescriptorSetLayout descriptorSetLayout;
		
		void CreateUniformBuffers();
	};
}