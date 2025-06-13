#pragma once

#include <memory>
#include <vector>
#include <string>

#include <volk.h>

#include <Core/SceneObject.h>
#include <Core/Transform.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	class Mesh;
	
	class MeshInstance : public SceneObject
	{
	public:
		MeshInstance(const std::string& name, std::shared_ptr<Mesh> mesh, VulkanDevice* device, VkDescriptorPool descriptorPool);
		~MeshInstance();

		std::shared_ptr<const Mesh> GetMesh() const;
		const std::vector<VkDescriptorSet>& GetUniformDescriptorSets() const;
		
		void UpdateUniformBuffer(uint32_t currentImage);

	private:
		VulkanDevice* device;

		std::vector<VkDescriptorSet> uniformDescriptorSets;

		std::vector<VulkanUniformBuffer> uniformBuffers;
		
		std::shared_ptr<Mesh> mesh;

		void CreateUniformBuffers();
		void CreateUniformDescriptorSets(VkDescriptorPool descriptorPool);
	};
}