#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <Vertex.h>
#include <VulkanUniformBuffer.h>

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
		bool enableTransparency = false;
	};
	
	class Mesh
	{
	public:
		Mesh(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, const std::string& name, const MeshInfo& info);
		~Mesh();
		
		const size_t GetIndicesSize() const;
		const std::string& GetName() const;

		VkDescriptorImageInfo GetBaseColorDescriptorInfo() const;
		VkDescriptorImageInfo GetRoughnessDescriptorInfo() const;
		VkDescriptorImageInfo GetMetallicDescriptorInfo() const;

		VkDescriptorSetLayout GetDescriptorSetLayout() const;
		
		bool GetTransparencyEnabled() const;

		VulkanBuffer* vertexBuffer;
		VulkanBuffer* indexBuffer;

	private:
		VulkanDevice* device;
		
		VulkanTexture* baseColorTexture;
		VulkanTexture* roughnessTexture;
		VulkanTexture* metallicTexture;
		
		VkDescriptorSetLayout descriptorSetLayout;

		bool transparencyEnabled = false;

		size_t indicesSize;

		std::string name;

		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		void CreateIndexBuffer(const std::vector<uint16_t>& indices);
	};
}