#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <Vulkan/Texture.h>

#include <volk.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
		ModelManager(VulkanDevice* device, VkDescriptorSetLayout uniformDescriptorSetLayout, VkDescriptorSetLayout materialDescriptorSetLayout, VkDescriptorPool descriptorPool);
		~ModelManager();

		const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels();
		
		std::shared_ptr<Model> GetModel(const std::string& name);
		
		std::shared_ptr<Model> LoadModel(const std::string& name, const std::filesystem::path& path);
		
		void LoadTextures(std::shared_ptr<Model>& model);

	private:
		VulkanDevice* device;
		
		VkDescriptorSetLayout uniformDescriptorSetLayout;
		VkDescriptorSetLayout materialDescriptorSetLayout;

		VkDescriptorPool descriptorPool;
		
		std::unordered_map<std::string, std::shared_ptr<Model>> models;

		std::shared_ptr<VulkanTexture> fallbackTexture;
		
		std::shared_ptr<VulkanTexture> CreateFallbackTexture(glm::vec4 color);
		
		bool DecodeImage(const fastgltf::Asset& asset, const fastgltf::Image& image, std::vector<uint8_t>& outPixels, int& outWidth, int& outHeight, int& outChannels);
	};
}