#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class Mesh;
	class MeshInstance;
	struct MeshInfo;
	struct Transform;

	class MeshManager
	{
	public:
		MeshManager(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);
		~MeshManager();

		std::shared_ptr<Mesh> LoadMesh(const std::string& name, const MeshInfo& info);
		void CreateInstance(const std::string& name, const Transform& transform);

		void UpdateUniformBuffers(int currentFrame, VkExtent2D swapChainExtent);

		const std::vector<std::unique_ptr<MeshInstance>>& GetOpaqueMeshes() const;
		const std::vector<std::unique_ptr<MeshInstance>>& GetTransparentMeshes() const;

	private:
		VulkanDevice* device;
		
		VkDescriptorSetLayout descriptorSetLayout;
		
		VkDescriptorPool descriptorPool;

		std::vector<std::unique_ptr<MeshInstance>> opaqueMeshes;
		std::vector<std::unique_ptr<MeshInstance>> transparentMeshes;

		std::unordered_set<std::string> meshInstanceNames;
		
		std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
	};
}