#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <Vertex.h>
#include <VulkanUniformBuffer.h>
#include <Transform.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanTexture;

	struct MeshInfo
	{
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;
		std::string baseColorPath;
		std::string roughnessPath;
		std::string metallicPath;
	};
	
	class Mesh
	{
	public:
		Mesh(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, const MeshInfo& info);
		~Mesh();

		void CreateDescriptorSets(VkDescriptorPool descriptorPool);

		void UpdateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent);

		size_t GetIndicesSize() const;

		VulkanBuffer* vertexBuffer;
		VulkanBuffer* indexBuffer;

		std::vector<VkDescriptorSet> descriptorSets;

		Transform transform;

	private:
		VulkanDevice* device;
		
		VulkanTexture* baseColorTexture;
		VulkanTexture* roughnessTexture;
		VulkanTexture* metallicTexture;

		std::vector<VulkanUniformBuffer> uniformBuffers;

		VkDescriptorSetLayout descriptorSetLayout;

		size_t indicesSize;

		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		void CreateIndexBuffer(const std::vector<uint16_t>& indices);
		void CreateUniformBuffers();
	};
}