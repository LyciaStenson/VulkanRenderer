#pragma once

#include <ImGuiWindow.h>

namespace VulkanRenderer
{
	class ModelManager;
	class Scene;
	
	class InstantiateModelWindow : public ImGuiWindow
	{
	public:
		InstantiateModelWindow(ModelManager* modelManager, Scene* scene, bool open = false);

	protected:
		void OnRender() override;

		ModelManager* m_ModelManager = nullptr;
		Scene* m_Scene = nullptr;
	};
}