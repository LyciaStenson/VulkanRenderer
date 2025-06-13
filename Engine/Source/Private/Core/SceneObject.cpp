#include <SceneObject.h>

using namespace VulkanRenderer;

SceneObject::SceneObject(const std::string& name)
	: name(name)
{
	this->transform.owner = this;
}

const std::string& SceneObject::GetName() const
{
	return name;
}