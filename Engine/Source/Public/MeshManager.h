#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class Transform;
	class Mesh;
	class MeshInstance;
	struct MeshInfo;

	class MeshManager
	{
	public:
		MeshManager(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout);
		~MeshManager();

		std::shared_ptr<Mesh> GetMesh(const std::string& name);

		std::shared_ptr<Mesh> LoadMesh(const std::string& name, const MeshInfo& info);

	private:
		VulkanDevice* device;
		
		VkDescriptorSetLayout descriptorSetLayout;
		
		std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
	};
}