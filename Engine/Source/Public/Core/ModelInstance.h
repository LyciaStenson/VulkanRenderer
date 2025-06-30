#pragma once

#include <string>

#include <Core/SceneObject.h>
#include <Core/Transform.h>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	class Mesh;

	class ModelInstance : public SceneObject
	{
	public:
		ModelInstance() = default;
		ModelInstance(const std::string& name, const std::string& modelPath);
		~ModelInstance();
		
		const std::string& GetModelPath() const;

	private:
		std::string modelPath;
	};
}

CEREAL_REGISTER_TYPE(VulkanRenderer::ModelInstance)
CEREAL_REGISTER_POLYMORPHIC_RELATION(VulkanRenderer::SceneObject, VulkanRenderer::ModelInstance)