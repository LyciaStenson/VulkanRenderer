#include <Core/Transform.h>

using namespace VulkanRenderer;

Transform::Transform()
	: position(0.0f), rotation(), scale(1.0f)
{

}

Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
	: position(position), rotation(rotation), scale(scale)
{

}

Transform::~Transform()
{
	SetParent(nullptr);
	for (Transform* child : children)
	{
		child->parent = nullptr;
	}
	children.clear();
}

void Transform::SetParent(Transform* newParent)
{
	if (parent == newParent)
		return;

	if (parent)
		parent->RemoveChild(this);

	parent = newParent;

	if (parent)
		parent->AddChild(this);
}

Transform* Transform::GetParent() const
{
	return parent;
}

const std::vector<Transform*>& Transform::GetChildren() const
{
	return children;
}

glm::mat4 Transform::GetLocalMatrix() const
{
	glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 r = glm::mat4_cast(rotation);
	glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
	return t * r * s;
}

glm::mat4 Transform::GetWorldMatrix() const
{
	if (parent)
		return parent->GetWorldMatrix() * GetLocalMatrix();
	else
		return GetLocalMatrix();
}

void Transform::AddChild(Transform* child)
{
	if (std::find(children.begin(), children.end(), child) == children.end())
		children.push_back(child);
}

void Transform::RemoveChild(Transform* child)
{
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end())
		children.erase(it);
}