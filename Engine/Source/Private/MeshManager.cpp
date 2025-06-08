#include <MeshManager.h>

#include <iostream>

#include <Transform.h>
#include <Mesh.h>
#include <MeshInstance.h>

using namespace VulkanRenderer;

MeshManager::MeshManager(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
	: device(device), descriptorSetLayout(descriptorSetLayout), descriptorPool(descriptorPool)
{

}

MeshManager::~MeshManager()
{
	meshes.clear();
}

std::shared_ptr<Mesh> MeshManager::LoadMesh(const std::string& name, const MeshInfo& info)
{
	auto it = meshes.find(name);
	if (it != meshes.end())
	{
		return it->second;
	}

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(device, descriptorSetLayout, name, info);
	meshes[name] = mesh;

	return mesh;
}

MeshInstance* MeshManager::CreateInstance(const std::string& name, const Transform& transform)
{
	auto mesh = meshes[name];
	if (!mesh)
	{
		std::cout << "Mesh " << name << " not found" << std::endl;
		return nullptr;
	}

	std::string instanceName = name;
	int counter = 1;
	while (meshInstanceNames.count(instanceName))
	{
		instanceName = name + std::to_string(counter);
		counter++;
	}
	meshInstanceNames.insert(instanceName);
	
	std::unique_ptr<MeshInstance> instance = std::make_unique<MeshInstance>(device, descriptorPool, mesh, transform, instanceName);
	MeshInstance* instancePtr = instance.get();

	if (mesh->GetTransparencyEnabled())
		transparentMeshes.push_back(std::move(instance));
	else
		opaqueMeshes.push_back(std::move(instance));

	return instancePtr;
}

void MeshManager::UpdateUniformBuffers(int currentFrame, VkExtent2D swapChainExtent)
{
	for (auto& mesh : opaqueMeshes)
	{
		mesh->UpdateUniformBuffer(currentFrame, swapChainExtent);
	}
	for (auto& mesh : transparentMeshes)
	{
		mesh->UpdateUniformBuffer(currentFrame, swapChainExtent);
	}
}

const std::vector<std::unique_ptr<MeshInstance>>& MeshManager::GetOpaqueMeshes() const
{
	return opaqueMeshes;
}

const std::vector<std::unique_ptr<MeshInstance>>& MeshManager::GetTransparentMeshes() const
{
	return transparentMeshes;
}

std::vector<std::unique_ptr<MeshInstance>>& MeshManager::GetOpaqueMeshesMutable()
{
	return opaqueMeshes;
}

std::vector<std::unique_ptr<MeshInstance>>& MeshManager::GetTransparentMeshesMutable()
{
	return transparentMeshes;
}