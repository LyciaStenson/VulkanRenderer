#pragma once

#include <vector>
#include <memory>

#include <ImGui/ImGuiWindow.h>
#include <Core/SceneObject.h>

namespace VulkanRenderer
{
	class VulkanImGuiOverlay;
	class Scene;
	
	class SceneOutliner : public ImGuiWindow
	{
	public:
		SceneOutliner(Scene* scene, VulkanImGuiOverlay* overlay, bool open = true);

	protected:
		void OnRender() override;
		
		void SceneOutliner::DrawSceneNode(SceneObject* object);

		Scene* m_Scene = nullptr;
		VulkanImGuiOverlay* m_Overlay = nullptr;
	};
}