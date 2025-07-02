#pragma once

#include <vector>
#include <memory>
#include <string>

#include <Core/Transform.h>

#include <cereal/cereal.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace VulkanRenderer
{
	class SceneObject
	{
	public:
		SceneObject() = default;
		SceneObject(const std::string& name);
		virtual ~SceneObject();

		const std::string& GetName() const;
		std::string GetPath() const;
		
		void SetParent(SceneObject* transform);
		SceneObject* GetParent() const;
		
		const std::vector<std::unique_ptr<SceneObject>>& GetChildren() const;

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix() const;

		void AddChild(std::unique_ptr<SceneObject> child);
		std::unique_ptr<SceneObject> DetachChild(SceneObject* child);

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				CEREAL_NVP(name),
				CEREAL_NVP(transform),
				CEREAL_NVP(children)
			);
		}
		
		Transform transform;

		SceneObject* parent = nullptr;

	protected:
		std::string name;
		
		std::vector<std::unique_ptr<SceneObject>> children;
	};
}

CEREAL_REGISTER_TYPE(VulkanRenderer::SceneObject)