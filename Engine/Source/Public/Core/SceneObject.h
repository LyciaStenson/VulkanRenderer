#pragma once

#include <vector>
#include <string>

#include <Core/Transform.h>

#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

namespace VulkanRenderer
{
	class SceneObject
	{
	public:
		SceneObject(const std::string& name);
		virtual ~SceneObject() = default;

		const std::string& GetName() const;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(name, transform);
		}
		
		Transform transform;

	private:
		std::string name;
	};
}