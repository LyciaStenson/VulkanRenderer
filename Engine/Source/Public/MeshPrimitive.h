#pragma once

#include <vector>
#include <memory>

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
		bool enableTransparency = false;
	};

	class MeshPrimitive
	{
	public:
		MeshPrimitive(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, const MeshPrimitiveInfo& info);
		~MeshPrimitive();
		
		const size_t GetIndicesSize() const;

		VkDescriptorImageInfo GetBaseColorDescriptorInfo() const;
		VkDescriptorImageInfo GetRoughnessDescriptorInfo() const;
		VkDescriptorImageInfo GetMetallicDescriptorInfo() const;

		const std::vector<VkDescriptorSet>& GetMaterialDescriptorSets() const;

		bool GetTransparencyEnabled() const;

		VulkanBuffer* vertexBuffer;
		VulkanBuffer* indexBuffer;

		std::shared_ptr<VulkanTexture> baseColorTexture;
		std::shared_ptr<VulkanTexture> roughnessTexture;
		std::shared_ptr<VulkanTexture> metallicTexture;

	private:
		VulkanDevice* device;

		bool transparencyEnabled = false;

		size_t indicesSize;

		VkDescriptorSetLayout materialDescriptorSetLayout;

		std::vector<VkDescriptorSet> materialDescriptorSets;

		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		void CreateIndexBuffer(const std::vector<uint16_t>& indices);
		
		void CreateUniformDescriptorSets(VkDescriptorPool descriptorPool);
	};
}