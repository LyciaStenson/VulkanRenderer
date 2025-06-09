#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <volk.h>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

namespace VulkanRenderer
{
	class VulkanDevice;
	class SceneObject;
	class MeshInstance;
	class ModelManager;
	class Mesh;
	class Camera;
	class Transform;
	struct Model;

	class Scene
	{
	public:
		Scene(VulkanDevice* device, ModelManager* modelManager, VkDescriptorSetLayout cameraDescriptorSetLayout, VkDescriptorPool descriptorPool);
		~Scene();

		const std::vector<std::unique_ptr<SceneObject>>& GetObjects() const;
		std::vector<std::unique_ptr<SceneObject>>& GetObjectsMutable();
		
		SceneObject* CreateSceneObject(const std::string& name, const Transform& transform);
		Camera* CreateCamera(const std::string& name, const Transform& transform);

		void InstantiateModel(const std::string& name, const Transform& transform);
		
		void UpdateUniformBuffers(int currentFrame, VkExtent2D swapChainExtent);

	private:
		VulkanDevice* device;
		
		VkDescriptorSetLayout cameraDescriptorSetLayout;

		VkDescriptorPool descriptorPool;

		ModelManager* modelManager;

		std::vector<std::unique_ptr<SceneObject>> objects;
		std::unordered_set<std::string> objectNames;

		void InstantiateModelNode(const std::shared_ptr<Model>& model, const fastgltf::Node& node);
		
		MeshInstance* CreateMeshInstance(const std::string& name, const Transform& transform, std::shared_ptr<Mesh> mesh);
	};
}