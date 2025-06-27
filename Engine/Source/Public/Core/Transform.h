#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Serialization/GlmSerialization.h>

namespace VulkanRenderer
{
	class SceneObject;

	class Transform
	{
	public:
		Transform();
		Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
		~Transform();
		
		void SetParent(Transform* transform);
		Transform* GetParent() const;

		const std::vector<Transform*>& GetChildren() const;

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix() const;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				cereal::make_nvp("position", position),
				cereal::make_nvp("rotation", rotation),
				cereal::make_nvp("scale", scale)
			);
		}

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;

		SceneObject* owner = nullptr;

	private:
		Transform* parent = nullptr;
		std::vector<Transform*> children;
		
		void AddChild(Transform* child);
		void RemoveChild(Transform* child);
	};
}