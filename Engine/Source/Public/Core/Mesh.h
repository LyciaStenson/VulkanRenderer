#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <Core/Vertex.h>
#include <Vulkan/UniformBuffer.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanTexture;
	class MeshPrimitive;
	struct MeshPrimitiveInfo;
	
	class Mesh
	{
	public:
		Mesh(VulkanDevice* device, VkDescriptorSetLayout uniformDescriptorSetLayout);
		~Mesh();

		size_t GetPrimitiveCount() const;
		MeshPrimitive* GetPrimitive(size_t index) const;
		
		VkDescriptorSetLayout GetUniformDescriptorSetLayout() const;
		
		void AddPrimitive(std::unique_ptr<MeshPrimitive> meshPrimitive);

	private:
		VulkanDevice* device;
		
		std::vector<std::unique_ptr<MeshPrimitive>> primitives;

		VkDescriptorSetLayout uniformDescriptorSetLayout;
	};
}