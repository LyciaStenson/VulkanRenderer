#pragma once

#include <filesystem>

#include <ImGuiWindow.h>

namespace VulkanRenderer
{
	class Scene;
	class VulkanImGuiOverlay;

	class AssetBrowser : public ImGuiWindow
	{
	public:
		AssetBrowser(Scene* scene, VulkanImGuiOverlay* overlay, bool open = true);

	protected:
		void OnRender() override;

		Scene* m_Scene = nullptr;

		VulkanImGuiOverlay* m_Overlay;
		
		std::filesystem::path m_CurrentPath;
		std::filesystem::path m_SelectedPath;
	};
}