#pragma once

#include <vector>

#include <Vertex.h>
#include <Texture.h>

namespace VulkanRenderer
{
	struct MeshPrimitive
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		std::vector<Texture> textures;
	};
}