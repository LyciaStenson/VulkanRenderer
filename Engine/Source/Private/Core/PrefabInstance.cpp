#include <Core/PrefabInstance.h>

using namespace VulkanRenderer;

PrefabInstance::PrefabInstance(const std::string& name, const std::string& prefabPath)
	: SceneObject(name), prefabPath(prefabPath)
{

}

PrefabInstance::~PrefabInstance()
{

}

const std::string& PrefabInstance::GetPrefabPath() const
{
	return prefabPath;
}