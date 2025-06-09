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
	class VulkanTexture;
	class Transform;
	class Mesh;
	class MeshInstance;
	struct MeshInfo;
	struct Model;
	
	class ModelManager
	{
	public:
		ModelManager(VulkanDevice* device, VkDescriptorSetLayout uniformDescriptorSetLayout);
		~ModelManager();
		
		std::shared_ptr<Model> GetModel(const std::string& name);
		
		std::shared_ptr<Model> LoadModel(const std::string& name, const std::filesystem::path& path);
		
		void LoadTextures(std::shared_ptr<Model>& model);

	private:
		VulkanDevice* device;
		
		VkDescriptorSetLayout uniformDescriptorSetLayout;
		
		std::unordered_map<std::string, std::shared_ptr<Model>> models;

		bool DecodeImage(const fastgltf::Asset& asset, const fastgltf::Image& image, std::vector<uint8_t>& outPixels, int& outWidth, int& outHeight, int& outChannels);
	};
}