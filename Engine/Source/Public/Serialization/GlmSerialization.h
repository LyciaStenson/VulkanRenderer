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
			cereal::make_nvp("x", vec.x),
			cereal::make_nvp("y", vec.y),
			cereal::make_nvp("z", vec.z)
		);
	}

	template <class Archive>
	inline void serialize(Archive& archive, glm::quat& quat)
	{
		archive
		(
			cereal::make_nvp("x", quat.x),
			cereal::make_nvp("y", quat.y),
			cereal::make_nvp("z", quat.z),
			cereal::make_nvp("w", quat.w)
		);
	}
}