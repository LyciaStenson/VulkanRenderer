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
		virtual ~SceneObject();

		const std::string& GetName() const;
		
		void SetParent(SceneObject* transform);
		SceneObject* GetParent() const;
		
		const std::vector<std::unique_ptr<SceneObject>>& GetChildren() const;

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix() const;

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

		SceneObject* parent = nullptr;

	private:
		std::string name;
		
		std::vector<std::unique_ptr<SceneObject>> children;
		
		void AddChild(std::unique_ptr<SceneObject> child);
		std::unique_ptr<SceneObject> DetachChild(SceneObject* child);
	};
}

CEREAL_REGISTER_TYPE(VulkanRenderer::SceneObject)