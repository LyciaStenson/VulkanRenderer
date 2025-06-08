#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <VulkanTexture.h>

#include <volk.h>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

namespace VulkanRenderer
{
	class VulkanDevice;
	class Transform;
	class Mesh;
	class MeshInstance;
	struct MeshInfo;
	struct Model;
	class VulkanTexture;

	class MeshManager
	{
	public:
		MeshManager(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout);
		~MeshManager();
		
		std::shared_ptr<Mesh> GetMesh(const std::string& name);
		
		std::shared_ptr<Model> LoadModel(const std::string& name, const std::filesystem::path& path);
		
		std::shared_ptr<Mesh> LoadMesh(const std::string& name, const MeshInfo& info);

		void LoadTextures(const fastgltf::Asset& asset);
		VulkanTexture* LoadTexture(const fastgltf::Asset& asset, const fastgltf::Image& image, bool sRGB);

	private:
		VulkanDevice* device;
		
		VkDescriptorSetLayout descriptorSetLayout;
		
		std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;

		std::unordered_map<std::string, std::shared_ptr<Model>> models;

		std::unordered_map<size_t, VulkanTexture*> materialTextures;
	};
}