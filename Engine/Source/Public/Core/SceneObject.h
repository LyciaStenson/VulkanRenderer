#pragma once

#include <vector>
#include <string>

#include <Core/Transform.h>

#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace VulkanRenderer
{
	class SceneObject
	{
	public:
		SceneObject() = default;
		SceneObject(const std::string& name);
		virtual ~SceneObject() = default;

		const std::string& GetName() const;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				CEREAL_NVP(name),
				CEREAL_NVP(transform)
			);
		}
		
		Transform transform;

	private:
		std::string name;
	};
}

CEREAL_REGISTER_TYPE(VulkanRenderer::SceneObject)