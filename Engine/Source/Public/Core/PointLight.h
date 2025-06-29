#pragma once

#include <string>

#include <Core/SceneObject.h>

#include <volk.h>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <Serialization/GlmSerialization.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	struct PointLightData;
	
	class PointLight : public SceneObject
	{
	public:
		PointLight() = default;
		PointLight(const std::string& name);
		~PointLight();

		PointLightData GetData() const;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				cereal::base_class<SceneObject>(this),
				CEREAL_NVP(color),
				CEREAL_NVP(intensity),
				CEREAL_NVP(radius)
			);
		}
		
		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
		float radius = 10.0f;
	};
}

CEREAL_REGISTER_TYPE(VulkanRenderer::PointLight)
CEREAL_REGISTER_POLYMORPHIC_RELATION(VulkanRenderer::SceneObject, VulkanRenderer::PointLight)