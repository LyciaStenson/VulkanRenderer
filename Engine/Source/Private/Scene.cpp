#include <Scene.h>

#include <iostream>

#include <SceneObject.h>
#include <MeshInstance.h>
#include <ModelManager.h>
#include <Mesh.h>
#include <Camera.h>
#include <Transform.h>
#include <Model.h>

using namespace VulkanRenderer;

Scene::Scene(VulkanDevice* device, ModelManager* modelManager, VkDescriptorSetLayout cameraDescriptorSetLayout, VkDescriptorPool descriptorPool)
	: device(device), modelManager(modelManager), cameraDescriptorSetLayout(cameraDescriptorSetLayout), descriptorPool(descriptorPool)
{

}

Scene::~Scene()
{

}

const std::vector<std::unique_ptr<SceneObject>>& Scene::GetObjects() const
{
	return objects;
}

std::vector<std::unique_ptr<SceneObject>>& Scene::GetObjectsMutable()
{
	return objects;
}

SceneObject* Scene::CreateSceneObject(const std::string& name, const Transform& transform)
{
	std::string instanceName = name;
	int counter = 1;
	while (objectNames.count(instanceName))
	{
		instanceName = name + std::to_string(counter);
		++counter;
	}
	objectNames.insert(instanceName);

	std::unique_ptr<SceneObject> object = std::make_unique<SceneObject>(instanceName, transform);
	SceneObject* objectPtr = object.get();
	objects.push_back(std::move(object));

	return objectPtr;
}

Camera* Scene::CreateCamera(const std::string& name, const Transform& transform)
{
	std::string cameraName = name;
	int counter = 1;
	while (objectNames.count(cameraName))
	{
		cameraName = name + std::to_string(counter);
		++counter;
	}
	objectNames.insert(cameraName);

	std::unique_ptr<Camera> camera = std::make_unique<Camera>(cameraName, transform, device, cameraDescriptorSetLayout);
	Camera* cameraPtr = camera.get();
	objects.push_back(std::move(camera));
	
	cameraPtr->CreateDescriptorSets(descriptorPool);

	return cameraPtr;
}

MeshInstance* Scene::CreateMeshInstance(const std::string& name, const Transform& transform, std::shared_ptr<Mesh> mesh)
{
	std::string instanceName = name;
	int counter = 1;
	while (objectNames.count(instanceName))
	{
		instanceName = name + std::to_string(counter);
		++counter;
	}
	objectNames.insert(instanceName);
	
	std::unique_ptr<MeshInstance> meshInstance = std::make_unique<MeshInstance>(instanceName, transform, mesh, device, descriptorPool);
	MeshInstance* meshInstancePtr = meshInstance.get();
	objects.push_back(std::move(meshInstance));

	return meshInstancePtr;
}

void Scene::InstantiateModelNode(const std::shared_ptr<Model>& model, const fastgltf::Node& node)
{
	fastgltf::math::fvec3 translation(0.0f, 0.0f, 0.0f);
	fastgltf::math::fquat rotation;
	fastgltf::math::fvec3 scale(1.0f, 1.0f, 1.0f);

	if (auto* transform = std::get_if<fastgltf::math::fmat4x4>(&node.transform))
	{
		fastgltf::math::decomposeTransformMatrix(*transform, scale, rotation, translation);
	}
	else if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform))
	{
		translation = trs->translation;
		rotation = trs->rotation;
		scale = trs->scale;
	}

	Transform transform
	{
		glm::vec3(translation.x(), translation.y(), translation.z()),
		glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z()),
		glm::vec3(scale.x(), scale.y(), scale.z())
	};

	SceneObject* object = nullptr;

	if (node.meshIndex.has_value())
	{
		object = CreateMeshInstance(node.name.c_str(), transform, model->meshes[node.meshIndex.value()]);
	}
	else
	{
		object = CreateSceneObject(node.name.c_str(), transform);
	}

	for (const auto& childNodeIndex : node.children)
	{
		const auto& childNode = model->gltfAsset.nodes[childNodeIndex];
		InstantiateModelNode(model, childNode);
	}
}

void Scene::InstantiateModel(const std::string& name, const Transform& transform)
{
	const auto& model = modelManager->GetModel(name);

	if (!model)
	{
		std::cout << "Model " << name << " not found. Make sure model is loaded first." << std::endl;
		return;
	}

	const fastgltf::Scene& gltfScene = model->gltfAsset.scenes[0];

	for (size_t rootNodeIndex : gltfScene.nodeIndices)
	{
		InstantiateModelNode(model, model->gltfAsset.nodes[rootNodeIndex]);
	}
}

void Scene::UpdateUniformBuffers(int currentFrame, VkExtent2D swapChainExtent)
{
	for (const auto& object : GetObjects())
	{
		if (auto* meshInstance = dynamic_cast<MeshInstance*>(object.get()))
		{
			meshInstance->UpdateUniformBuffer(currentFrame, swapChainExtent);
		}
		if (auto* camera = dynamic_cast<Camera*>(object.get()))
		{
			camera->UpdateUniformBuffer(currentFrame, swapChainExtent);
		}
	}
}