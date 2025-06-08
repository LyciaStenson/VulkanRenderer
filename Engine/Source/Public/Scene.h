#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <volk.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class SceneObject;
	class MeshInstance;
	class MeshManager;
	class Camera;
	class Transform;

	class Scene
	{
	public:
		Scene(VulkanDevice* device, MeshManager* meshManager, VkDescriptorSetLayout cameraDescriptorSetLayout, VkDescriptorPool descriptorPool);
		~Scene();

		const std::vector<std::unique_ptr<SceneObject>>& GetObjects() const;
		std::vector<std::unique_ptr<SceneObject>>& GetObjectsMutable();
		
		SceneObject* CreateSceneObject(const std::string& name, const Transform& transform);
		Camera* CreateCamera(const std::string& name, const Transform& transform);
		MeshInstance* CreateMeshInstance(const std::string& name, const Transform& transform);

		void UpdateUniformBuffers(int currentFrame, VkExtent2D swapChainExtent);

	private:
		VulkanDevice* device;

		//VkDescriptorSetLayout meshDescriptorSetLayout;
		VkDescriptorSetLayout cameraDescriptorSetLayout;

		VkDescriptorPool descriptorPool;

		MeshManager* meshManager;

		std::vector<std::unique_ptr<SceneObject>> objects;
		std::unordered_set<std::string> objectNames;
	};
}