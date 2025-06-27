#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cereal/cereal.hpp>

namespace glm
{
	template <class Archive>
	inline void serialize(Archive& archive, glm::vec3& vec)
	{
		archive
		(
			CEREAL_NVP(vec.x),
			CEREAL_NVP(vec.y),
			CEREAL_NVP(vec.z)
		);
	}

	template <class Archive>
	inline void serialize(Archive& archive, glm::quat& quat)
	{
		archive
		(
			CEREAL_NVP(quat.x),
			CEREAL_NVP(quat.y),
			CEREAL_NVP(quat.z),
			CEREAL_NVP(quat.w)
		);
	}
}