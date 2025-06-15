#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <volk.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanStorageBuffer;
	class SceneObject;
	class MeshInstance;
	class ModelManager;
	class Mesh;
	class Camera;
	class PointLight;
	class Transform;
	struct Model;

	class Scene
	{
	public:
		Scene(VulkanDevice* device, ModelManager* modelManager, VkDescriptorSetLayout cameraDescriptorSetLayout, VkDescriptorPool descriptorPool);
		~Scene();

		const std::vector<std::unique_ptr<SceneObject>>& GetObjects() const;
		std::vector<std::unique_ptr<SceneObject>>& GetObjectsMutable();

		Camera* GetMainCamera() const;
		void SetMainCamera(Camera* camera);
		
		SceneObject* CreateSceneObject(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent);
		Camera* CreateCamera(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent);
		PointLight* CreatePointLight(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent);

		SceneObject* InstantiateModel(const std::string& name, const Transform& transform);
		
		void UpdateUniformBuffers(int currentFrame, VkExtent2D swapChainExtent);

	private:
		VulkanDevice* device;
		
		VkDescriptorSetLayout cameraDescriptorSetLayout;

		VkDescriptorPool descriptorPool;

		ModelManager* modelManager;

		std::vector<std::unique_ptr<SceneObject>> objects;
		std::unordered_set<std::string> objectNames;

		std::unique_ptr<VulkanStorageBuffer> pointLightBuffer;

		Camera* mainCamera = nullptr;

		void InstantiateModelNode(const std::shared_ptr<Model>& model, const fastgltf::Node& node, Transform* parent);
		
		MeshInstance* CreateMeshInstance(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent, std::shared_ptr<Mesh> mesh);

		//void CreateLightBuffer(uint32_t maxLights);
		//void UpdateLightBuffer();
	};
}