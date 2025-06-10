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
		//glm::vec2 texCoord;
		glm::vec2 baseColorTexCoord;
		glm::vec2 metallicRoughnessTexCoord;
		glm::vec2 normalTexCoord;

		static VkVertexInputBindingDescription GetBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();
	};
}