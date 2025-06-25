#include <Core/PointLight.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Vulkan/UniformBuffer.h>
#include <Core/PointLightData.h>

using namespace VulkanRenderer;

PointLight::PointLight(const std::string& name)
	: SceneObject(name)
{

}

PointLight::~PointLight()
{

}

PointLightData PointLight::GetData() const
{
	PointLightData data;
	data.positionRadius = glm::vec4(transform.position, radius);
	data.colorIntensity = glm::vec4(color, intensity);

	return data;
}