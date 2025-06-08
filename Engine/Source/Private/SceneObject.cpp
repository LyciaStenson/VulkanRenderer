#include <SceneObject.h>

using namespace VulkanRenderer;

SceneObject::SceneObject(const std::string& name, const Transform& transform)
	: name(name), transform(transform)
{
	this->transform.owner = this;
}

const std::string& SceneObject::GetName() const
{
	return name;
}