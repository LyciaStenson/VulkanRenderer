#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <Core/SceneObject.h>
#include <Core/Transform.h>
#include <Vulkan/UniformBuffer.h>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace VulkanRenderer
{
	struct CameraUBO;
	
	class Camera : public SceneObject
	{
	public:
		Camera() = default;
		Camera(const std::string& name);
		~Camera();
		
		CameraUBO GetUBO(VkExtent2D swapChainExtent) const;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				cereal::base_class<SceneObject>(this),
				CEREAL_NVP(fov)
			);
		}
		
		float fov = 70.0f;
	};
}

CEREAL_REGISTER_TYPE(VulkanRenderer::Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(VulkanRenderer::SceneObject, VulkanRenderer::Camera)