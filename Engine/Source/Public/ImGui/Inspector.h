#pragma once

#include <ImGui/ImGuiWindow.h>

namespace VulkanRenderer
{
	class VulkanImGuiOverlay;
	class SceneObject;
	class Scene;

	class Inspector : public ImGuiWindow
	{
	public:
		Inspector(Scene* scene, VulkanImGuiOverlay* overlay, bool open = true);

	protected:
		void OnRender() override;
		
		Scene* m_Scene = nullptr;
		VulkanImGuiOverlay* m_Overlay = nullptr;

		SceneObject* m_SelectedObject = nullptr;
	};
}