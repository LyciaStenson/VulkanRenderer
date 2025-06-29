#include <Core/Camera.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Core/CameraUBO.h>

using namespace VulkanRenderer;

Camera::Camera(const std::string& name)
	: SceneObject(name)
{

}

Camera::~Camera()
{

}

CameraUBO Camera::GetUBO(VkExtent2D swapChainExtent) const
{
	CameraUBO ubo{};
	
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform.position);
	glm::mat4 rotation = glm::mat4_cast(transform.rotation);
	glm::mat4 world = translation * rotation;
	
	ubo.view = glm::inverse(world);
	ubo.projection = glm::perspective(glm::radians(fov), (float)swapChainExtent.width / (float)swapChainExtent.height, 0.01f, 100.0f);
	ubo.projection[1][1] *= -1;

	return ubo;
}