#include <MeshManager.h>

#include <iostream>

#include <Transform.h>
#include <Mesh.h>
#include <MeshInstance.h>

using namespace VulkanRenderer;

MeshManager::MeshManager(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout)
	: device(device), descriptorSetLayout(descriptorSetLayout)
{

}

MeshManager::~MeshManager()
{
	meshes.clear();
}

std::shared_ptr<Mesh> MeshManager::GetMesh(const std::string& name)
{
	auto it = meshes.find(name);
	if (it != meshes.end())
		return it->second;
	return nullptr;
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