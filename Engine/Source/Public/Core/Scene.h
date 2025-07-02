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

#include <cereal/cereal.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanStorageBuffer;
	class GlobalDescriptorSetManager;
	class SceneObject;
	class PrefabInstance;
	class MeshInstance;
	class ModelManager;
	class Mesh;
	class Camera;
	class PointLight;
	class Transform;
	struct PointLightData;
	struct Model;

	class Scene
	{
	public:
		Scene(VulkanDevice* device, ModelManager* modelManager, GlobalDescriptorSetManager* globalDescriptorSetManager, VkDescriptorPool descriptorPool);
		~Scene();

		const SceneObject* GetRootObject() const;
		std::vector<SceneObject*> GetAllObjects();
		
		Camera* GetMainCamera() const;
		void SetMainCamera(Camera* camera);

		bool SaveSceneJSON(const std::string& path);
		bool SaveSceneBIN(const std::string& path);

		bool LoadSceneJSON(const std::string& path);
		bool LoadSceneBIN(const std::string& path);

		SceneObject* CreateSceneObject(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent = nullptr);
		Camera* CreateCamera(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent = nullptr);
		PointLight* CreatePointLight(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent = nullptr);
		
		void InstantiateModel(PrefabInstance* prefabInstance);
		PrefabInstance* InstantiateModel(const std::string& path, const Transform& transform);

		SceneObject* FindObject(const std::string& path, SceneObject* root = nullptr);

		void UpdateBuffers(int currentFrame, VkExtent2D swapChainExtent);
		void UpdateBuffersRecursive(int currentFrame, VkExtent2D swapChainExtent, SceneObject* object, std::vector<PointLightData>& pointLightData);

		template <class Archive>
		void save(Archive& archive) const
		{
			archive(CEREAL_NVP(rootObject));
			std::string mainCameraPath = mainCamera ? mainCamera->GetPath() : "";
			archive(CEREAL_NVP(mainCameraPath));
		}

		template <class Archive>
		void load(Archive& archive)
		{
			rootObject = nullptr;
			mainCamera = nullptr;

			archive(CEREAL_NVP(rootObject));
			std::string mainCameraPath;
			archive(CEREAL_NVP(mainCameraPath));
			
			if (!mainCameraPath.empty())
				mainCamera = dynamic_cast<Camera*>(FindObject(mainCameraPath));

			std::vector<SceneObject*> allObjects = GetAllObjects();
			for (SceneObject* object : allObjects)
			{
				if (auto* prefab = dynamic_cast<PrefabInstance*>(object))
				{
					modelManager->LoadModel(prefab->GetPrefabPath());
					InstantiateModel(prefab);
				}
			}
		}

	private:
		VulkanDevice* device;

		VkDescriptorPool descriptorPool;

		GlobalDescriptorSetManager* globalDescriptorSetManager;
		ModelManager* modelManager;
		
		std::unique_ptr<SceneObject> rootObject;
		
		Camera* mainCamera = nullptr;

		void InstantiateModelNode(const std::shared_ptr<Model>& model, const fastgltf::Node& node, SceneObject* parent);

		PrefabInstance* CreatePrefabInstance(const std::string& name, const std::string& path, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent = nullptr);
		MeshInstance* CreateMeshInstance(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent, std::shared_ptr<Mesh> mesh);

		void GetAllObjectsRecursive(SceneObject* root, std::vector<SceneObject*>& allObjects);
	};
}