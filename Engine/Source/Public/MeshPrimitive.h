#pragma once

#include <vector>
#include <memory>

#include <VulkanUniformBuffer.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanTexture;
	class VulkanBuffer;
	struct Vertex;

	struct MeshPrimitiveInfo
	{
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;

		glm::vec4 baseColorFactor;
		float metallicFactor;
		float roughnessFactor;

		std::shared_ptr<VulkanTexture> baseColorTexture;
		std::shared_ptr<VulkanTexture> metallicRoughnessTexture;
		std::shared_ptr<VulkanTexture> normalTexture;

		bool enableTransparency = false;
		bool doubleSided = false;
	};

	class MeshPrimitive
	{
	public:
		MeshPrimitive(VulkanDevice* device, VkDescriptorSetLayout materialDescriptorSetLayout, VkDescriptorPool descriptorPool, const MeshPrimitiveInfo& info);
		~MeshPrimitive();
		
		const size_t GetIndicesSize() const;

		VkDescriptorImageInfo GetBaseColorDescriptorInfo() const;
		VkDescriptorImageInfo GetMetallicRoughnessDescriptorInfo() const;
		VkDescriptorImageInfo GetNormalDescriptorInfo() const;

		const std::vector<VkDescriptorSet>& GetMaterialDescriptorSets() const;

		bool GetTransparencyEnabled() const;
		
		VulkanBuffer* vertexBuffer;
		VulkanBuffer* indexBuffer;

		glm::vec4 baseColorFactor;
		float metallicFactor;
		float roughnessFactor;

		std::shared_ptr<VulkanTexture> baseColorTexture;
		std::shared_ptr<VulkanTexture> metallicRoughnessTexture;
		std::shared_ptr<VulkanTexture> normalTexture;

	private:
		VulkanDevice* device;

		bool transparencyEnabled = false;

		size_t indicesSize;

		VkDescriptorSetLayout materialDescriptorSetLayout;

		std::vector<VkDescriptorSet> materialDescriptorSets;
		
		VulkanUniformBuffer* materialFactorsUniformBuffer;
		
		void CreateMaterialFactorsUniformBuffer();
		
		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		void CreateIndexBuffer(const std::vector<uint16_t>& indices);
		
		void CreateMaterialDescriptorSets(VkDescriptorPool descriptorPool);
	};
}