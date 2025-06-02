#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Transform
{
	glm::vec3 position{};
	glm::quat rotation{};
	glm::vec3 scale{1.0f, 1.0f, 1.0f};
};