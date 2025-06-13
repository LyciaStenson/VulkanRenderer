#pragma once

#include <ImGuiWindow.h>

namespace VulkanRenderer
{
	class ModelManager;

	class LoadModelWindow : public ImGuiWindow
	{
	public:
		LoadModelWindow(ModelManager* modelManager, bool open = false);

	protected:
		void OnRender() override;
		
		ModelManager* m_ModelManager = nullptr;
	};
}