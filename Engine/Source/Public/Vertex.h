#pragma once

#include <array>

#include <volk.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanRenderer
{
	struct Vertex
	{
		glm::vec3 position;
		//glm::vec3 normal;
		//glm::vec3 tangent;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription GetBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
	};
}