#pragma once

#include <Core/SceneObject.h>

namespace VulkanRenderer
{
	class VulkanDevice;
	
	class Light : public SceneObject
	{
	public:
		Light();
		~Light();

		void UpdateUniformBuffer(uint32_t currentImage);

	protected:
		VulkanDevice* device;
	};
}