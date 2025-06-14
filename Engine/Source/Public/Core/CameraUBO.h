#pragma once

#include <glm/glm.hpp>

namespace VulkanRenderer
{
	struct alignas(16) CameraUBO
	{
		alignas(16)	glm::mat4 view;
		alignas(16)	glm::mat4 proj;
	};
}