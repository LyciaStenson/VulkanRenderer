#include <Core/SceneObject.h>

#include <cereal/types/polymorphic.hpp>

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

CEREAL_REGISTER_TYPE(VulkanRenderer::SceneObject);