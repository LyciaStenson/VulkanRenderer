#include <Core/Scene.h>

#include <iostream>

#include <Vulkan/Device.h>
#include <Vulkan/StorageBuffer.h>
#include <Vulkan/GlobalDescriptorSetManager.h>
#include <Core/SceneObject.h>
#include <Core/PrefabInstance.h>
#include <Core/MeshInstance.h>
#include <Core/ModelManager.h>
#include <Core/Mesh.h>
#include <Core/Camera.h>
#include <Core/CameraUBO.h>
#include <Core/PointLight.h>
#include <Core/PointLightData.h>
#include <Core/Transform.h>
#include <Core/Model.h>

#include <filesystem>
#include <fstream>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

using namespace VulkanRenderer;

Scene::Scene(VulkanDevice* device, ModelManager* modelManager, GlobalDescriptorSetManager* globalDescriptorSetManager, VkDescriptorPool descriptorPool)
	: device(device), modelManager(modelManager), globalDescriptorSetManager(globalDescriptorSetManager), descriptorPool(descriptorPool)
{
	rootObject = std::make_unique<SceneObject>("Root");
}

Scene::~Scene()
{

}

const SceneObject* Scene::GetRootObject() const
{
	return rootObject.get();
}

Camera* Scene::GetMainCamera() const
{
	return mainCamera;
}

void Scene::SetMainCamera(Camera* camera)
{
	mainCamera = camera;
}

bool Scene::SaveSceneJSON(const std::string& path)
{
	std::ofstream os(path);
	if (!os.is_open())
	{
		std::cerr << "Failed to open scene for writing at " << path << std::endl;
		return false;
	}
	
	cereal::JSONOutputArchive archive(os);
	save(archive);

	return true;
}

bool Scene::SaveSceneBIN(const std::string& path)
{
	std::ofstream os(path, std::ios::binary);
	if (!os.is_open())
	{
		std::cerr << "Failed to open scene for writing at " << path << std::endl;
		return false;
	}
	
	cereal::BinaryOutputArchive archive(os);
	save(archive);

	return true;
}

bool Scene::LoadSceneJSON(const std::string& path)
{
	std::ifstream is(path);
	if (!is.is_open())
	{
		std::cerr << "Failed to open scene for reading at " << path << std::endl;
		return false;
	}
	
	cereal::JSONInputArchive archive(is);
	load(archive);

	return true;
}

bool Scene::LoadSceneBIN(const std::string& path)
{
	std::ifstream is(path, std::ios::binary);
	if (!is.is_open())
	{
		std::cerr << "Failed to open scene for reading at " << path << std::endl;
		return false;
	}
	
	cereal::BinaryInputArchive archive(is);
	load(archive);

	return true;
}

SceneObject* Scene::CreateSceneObject(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent)
{
	if (!parent)
		parent = rootObject.get();

	std::string instanceName = name;
	//int counter = 1;
	//while (objectNames.count(instanceName))
	//{
		//instanceName = name + std::to_string(counter);
		//++counter;
	//}
	//objectNames.insert(instanceName);

	std::unique_ptr<SceneObject> object = std::make_unique<SceneObject>(instanceName);
	object->transform.position = position;
	object->transform.rotation = rotation;
	object->transform.scale = scale;
	object->SetParent(parent);

	SceneObject* objectPtr = object.get();
	
	if (parent)
		parent->AddChild(std::move(object));
	else
		rootObject->AddChild(std::move(object));

	return objectPtr;
}

Camera* Scene::CreateCamera(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent)
{
	if (!parent)
		parent = rootObject.get();

	std::string cameraName = name;
	//int counter = 1;
	//while (objectNames.count(cameraName))
	//{
		//cameraName = name + std::to_string(counter);
		//++counter;
	//}
	//objectNames.insert(cameraName);

	std::unique_ptr<Camera> camera = std::make_unique<Camera>(cameraName);
	camera->transform.position = position;
	camera->transform.rotation = rotation;
	camera->transform.scale = scale;
	camera->SetParent(parent);
	
	Camera* cameraPtr = camera.get();

	if (parent)
		parent->AddChild(std::move(camera));
	else
		rootObject->AddChild(std::move(camera));
	
	if (!GetMainCamera())
	{
		SetMainCamera(cameraPtr);
	}
	
	return cameraPtr;
}

PointLight* Scene::CreatePointLight(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent)
{
	if (!parent)
		parent = rootObject.get();

	std::string lightName = name;
	//int counter = 1;
	//while (objectNames.count(lightName))
	//{
		//lightName = name + std::to_string(counter);
		//++counter;
	//}
	//objectNames.insert(lightName);

	std::unique_ptr<PointLight> light = std::make_unique<PointLight>(lightName);
	light->transform.position = position;
	light->transform.rotation = rotation;
	light->transform.scale = scale;
	light->SetParent(parent);
	
	PointLight* lightPtr = light.get();
	
	if (parent)
		parent->AddChild(std::move(light));
	else
		rootObject->AddChild(std::move(light));

	return lightPtr;
}

PrefabInstance* Scene::CreatePrefabInstance(const std::string& name, const std::string& path, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent)
{
	if (!parent)
		parent = rootObject.get();

	std::string instanceName = name;
	//int counter = 1;
	//while (objectNames.count(instanceName))
	//{
		//instanceName = name + std::to_string(counter);
		//++counter;
	//}
	//objectNames.insert(instanceName);

	std::unique_ptr<PrefabInstance> prefab = std::make_unique<PrefabInstance>(instanceName, path);
	prefab->transform.position = position;
	prefab->transform.rotation = rotation;
	prefab->transform.scale = scale;
	prefab->SetParent(parent);

	PrefabInstance* prefabPtr = prefab.get();

	if (parent)
		parent->AddChild(std::move(prefab));
	else
		rootObject->AddChild(std::move(prefab));

	return prefabPtr;
}

MeshInstance* Scene::CreateMeshInstance(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent, std::shared_ptr<Mesh> mesh)
{
	if (!parent)
		parent = rootObject.get();

	std::string instanceName = name;
	//int counter = 1;
	//while (objectNames.count(instanceName))
	//{
		//instanceName = name + std::to_string(counter);
		//++counter;
	//}
	//objectNames.insert(instanceName);
	
	std::unique_ptr<MeshInstance> meshInstance = std::make_unique<MeshInstance>(instanceName, mesh, device, descriptorPool);
	meshInstance->transform.position = position;
	meshInstance->transform.rotation = rotation;
	meshInstance->transform.scale = scale;
	meshInstance->SetParent(parent);

	MeshInstance* meshInstancePtr = meshInstance.get();
	
	if (parent)
		parent->AddChild(std::move(meshInstance));
	else
		rootObject->AddChild(std::move(meshInstance));
	
	return meshInstancePtr;
}

void Scene::InstantiateModelNode(const std::shared_ptr<Model>& model, const fastgltf::Node& node, SceneObject* parent)
{
	fastgltf::math::fvec3 gltfTranslation(0.0f, 0.0f, 0.0f);
	fastgltf::math::fquat gltfRotation;
	fastgltf::math::fvec3 gltfScale(1.0f, 1.0f, 1.0f);

	if (auto* transform = std::get_if<fastgltf::math::fmat4x4>(&node.transform))
	{
		fastgltf::math::decomposeTransformMatrix(*transform, gltfScale, gltfRotation, gltfTranslation);
	}
	else if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform))
	{
		gltfTranslation = trs->translation;
		gltfRotation = trs->rotation;
		gltfScale = trs->scale;
	}
	
	glm::vec3 position = glm::vec3(gltfTranslation.x(), gltfTranslation.y(), gltfTranslation.z());
	glm::quat rotation = glm::quat(gltfRotation.w(), gltfRotation.x(), gltfRotation.y(), gltfRotation.z());
	glm::vec3 scale = glm::vec3(gltfScale.x(), gltfScale.y(), gltfScale.z());
	
	SceneObject* object = nullptr;

	if (node.meshIndex.has_value())
	{
		if (node.meshIndex.value() < model->meshes.size())
		{
			object = CreateMeshInstance(node.name.c_str(), position, rotation, scale, parent, model->meshes[node.meshIndex.value()]);
		}
	}
	else
	{
		object = CreateSceneObject(node.name.c_str(), position, rotation, scale, parent);
	}

	for (const auto& childNodeIndex : node.children)
	{
		const auto& childNode = model->gltfAsset.nodes[childNodeIndex];
		if (object)
			InstantiateModelNode(model, childNode, object);
	}
}

SceneObject* Scene::InstantiateModel(const std::string& path, const Transform& transform)
{
	const auto& model = modelManager->GetModel(path);

	if (!model)
	{
		std::cout << "Model at " << path << " not found. Make sure model is loaded first." << std::endl;
		return nullptr;
	}

	const fastgltf::Scene& gltfScene = model->gltfAsset.scenes[0];

	PrefabInstance* root = CreatePrefabInstance("Model", path, transform.position, transform.rotation, transform.scale, nullptr);

	for (size_t rootNodeIndex : gltfScene.nodeIndices)
	{
		InstantiateModelNode(model, model->gltfAsset.nodes[rootNodeIndex], root);
	}

	return nullptr;
}

SceneObject* Scene::FindObject(const std::string& path, SceneObject* root)
{
	size_t start = 0;
	size_t end = path.find('/');

	SceneObject* node = root ? root : rootObject.get();

	while (true)
	{
		std::string token = path.substr(start, end - start);

		bool found = false;
		for (const auto& child : node->GetChildren())
		{
			if (child->GetName() == token)
			{
				node = child.get();
				found = true;
				break;
			}
		}

		if (!found)
			return nullptr;

		if (end == std::string::npos)
			break;

		start = end + 1;
		end = path.find('/', start);
	}

	return node;
}

void Scene::UpdateBuffers(int currentFrame, VkExtent2D swapChainExtent)
{
	std::vector<PointLightData> pointLightData;
	
	UpdateBuffersRecursive(currentFrame, swapChainExtent, rootObject.get(), pointLightData);

	globalDescriptorSetManager->UpdatePointLights(currentFrame, pointLightData);
}

void Scene::UpdateBuffersRecursive(int currentFrame, VkExtent2D swapChainExtent, SceneObject* object, std::vector<PointLightData>& pointLightData)
{
	if (!object)
		return;

	if (auto* meshInstance = dynamic_cast<MeshInstance*>(object))
	{
		meshInstance->UpdateUniformBuffer(currentFrame);
	}
	else if (auto* camera = dynamic_cast<Camera*>(object))
	{
		globalDescriptorSetManager->UpdateCamera(currentFrame, camera->GetUBO(swapChainExtent));
	}
	else if (auto* light = dynamic_cast<PointLight*>(object))
	{
		pointLightData.push_back(light->GetData());
	}

	for (const auto& child : object->GetChildren())
		UpdateBuffersRecursive(currentFrame, swapChainExtent, child.get(), pointLightData);
}