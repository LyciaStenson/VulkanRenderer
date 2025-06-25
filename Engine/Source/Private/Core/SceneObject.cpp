#include <Core/SceneObject.h>

using namespace VulkanRenderer;

SceneObject::SceneObject(const std::string& name)
	: name(name)
{
	transform.owner = this;
}

const std::string& SceneObject::GetName() const
{
	return name;
}