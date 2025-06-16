#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <Core/SceneObject.h>
#include <Core/Transform.h>
#include <Vulkan/UniformBuffer.h>

namespace VulkanRenderer
{
	struct CameraUBO;
	
	class Camera : public SceneObject
	{
	public:
		Camera(const std::string& name);
		~Camera();
		
		CameraUBO GetUBO(VkExtent2D swapChainExtent) const;
		float fov = 70.0f;
	};
}