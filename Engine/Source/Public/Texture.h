#pragma once

namespace VulkanRenderer
{
	enum class TextureType
	{
		Unknown,
		BaseColor,
		Normal,
		MetallicRoughness,
		Occlusion
	};

	struct Texture
	{
		unsigned int id;
		TextureType type;
	};
}