#include <Scene.h>

#include <iostream>

#include <SceneObject.h>
#include <MeshInstance.h>
#include <MeshManager.h>
#include <Transform.h>

using namespace VulkanRenderer;

Scene::Scene(VulkanDevice* device, MeshManager* meshManager, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
	: device(device), meshManager(meshManager), descriptorSetLayout(descriptorSetLayout), descriptorPool(descriptorPool)
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

MeshInstance* Scene::CreateMeshInstance(const std::string& name, const Transform& transform)
{
	auto mesh = meshManager->GetMesh(name);
	if (!mesh)
	{
		std::cout << "Mesh " << name << " not found" << std::endl;
		return nullptr;
	}

	std::string instanceName = name;
	int counter = 1;
	while (objectNames.count(instanceName))
	{
		instanceName = name + std::to_string(counter);
		counter++;
	}
	objectNames.insert(instanceName);
	
	std::unique_ptr<MeshInstance> instance = std::make_unique<MeshInstance>(device, descriptorPool, mesh, transform, instanceName);
	MeshInstance* instancePtr = instance.get();
	objects.push_back(std::move(instance));
	
	return instancePtr;
}

void Scene::UpdateUniformBuffers(int currentFrame, VkExtent2D swapChainExtent)
{
	for (const auto& object : GetObjects())
	{
		if (auto* meshInstance = dynamic_cast<MeshInstance*>(object.get()))
		{
			meshInstance->UpdateUniformBuffer(currentFrame, swapChainExtent);
		}
	}
}