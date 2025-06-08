#pragma once

#include <memory>
#include <vector>
#include <string>

#include <volk.h>

#include <SceneObject.h>
#include <Transform.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	class Mesh;
	
	class MeshInstance : public SceneObject
	{
	public:
		MeshInstance(const std::string& name, const Transform& transform, std::shared_ptr<Mesh> mesh, VulkanDevice* device, VkDescriptorPool descriptorPool);
		~MeshInstance();

		std::shared_ptr<const Mesh> GetMesh() const;
		const std::vector<VkDescriptorSet>& GetDescriptorSets() const;
		
		void UpdateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent);

	private:
		VulkanDevice* device;

		std::vector<VkDescriptorSet> descriptorSets;

		std::vector<VulkanUniformBuffer> uniformBuffers;
		
		std::shared_ptr<Mesh> mesh;

		void CreateUniformBuffers();
		void CreateDescriptorSets(VkDescriptorPool descriptorPool);
	};
}