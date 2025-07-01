#include <Core/SceneObject.h>

using namespace VulkanRenderer;

SceneObject::SceneObject(const std::string& name)
	: name(name)
{

}

SceneObject::~SceneObject()
{
	//SetParent(nullptr);
}

const std::string& SceneObject::GetName() const
{
	return name;
}

std::string SceneObject::GetPath() const
{
	if (!parent)
		return name;

	return parent->GetPath() + '/' + name;
}

void SceneObject::SetParent(SceneObject* newParent)
{
	if (parent == newParent)
		return;

	std::unique_ptr<SceneObject> uniquePtr = nullptr;
	if (parent)
		uniquePtr = parent->DetachChild(this);

	parent = newParent;
	
	if (newParent && uniquePtr)
		newParent->AddChild(std::move(uniquePtr));
}

SceneObject* SceneObject::GetParent() const
{
	return parent;
}

const std::vector<std::unique_ptr<SceneObject>>& SceneObject::GetChildren() const
{
	return children;
}

glm::mat4 SceneObject::GetLocalMatrix() const
{
	return transform.GetLocalMatrix();
}

glm::mat4 SceneObject::GetWorldMatrix() const
{
	if (parent)
		return parent->GetWorldMatrix() * transform.GetLocalMatrix();
	else
		return transform.GetLocalMatrix();
}

void SceneObject::AddChild(std::unique_ptr<SceneObject> child)
{
	child->parent = this;
	children.push_back(std::move(child));
}

std::unique_ptr<SceneObject> SceneObject::DetachChild(SceneObject* child)
{
	auto it = std::find_if(children.begin(), children.end(),
		[child](const std::unique_ptr<SceneObject>& existingChild)
		{
			return existingChild.get() == child;
		}
	);

	if (it != children.end())
	{
		std::unique_ptr<SceneObject> detachedChild = std::move(*it);
		children.erase(it);
		detachedChild->parent = nullptr;
		return detachedChild;
	}

	return nullptr;
}