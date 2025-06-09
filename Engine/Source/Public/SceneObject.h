#pragma once

#include <vector>
#include <string>

#include <Transform.h>

namespace VulkanRenderer
{
	class SceneObject
	{
	public:
		SceneObject(const std::string& name);
		virtual ~SceneObject() = default;

		const std::string& GetName() const;

		Transform transform;

	private:
		std::string name;
	};
}