#include <ImGui/AssetBrowser.h>

#include <iostream>

#include <Core/Scene.h>
#include <Vulkan/ImGuiOverlay.h>

using namespace VulkanRenderer;

AssetBrowser::AssetBrowser(Scene* scene, VulkanImGuiOverlay* overlay, bool open)
	: ImGuiWindow("Asset Browser", open), m_Scene(scene), m_Overlay(overlay)
{
	m_CurrentPath = std::filesystem::path("Assets");
}

void AssetBrowser::OnRender()
{
	if (m_CurrentPath.has_parent_path())
	{
		if (ImGui::Button("^ Up"))
		{
			m_CurrentPath = m_CurrentPath.parent_path();
			m_SelectedPath.clear();
		}
	}
	
	for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
	{
		const std::filesystem::path& path = entry.path();
		const std::string name = entry.path().filename().string();

		bool selected = path == m_SelectedPath;
		
		if (entry.is_directory())
		{
			if (ImGui::Selectable(name.c_str(), selected))
			{
				m_SelectedPath = path;
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_CurrentPath = path;
				m_SelectedPath.clear();
			}
		}
		else if (entry.is_regular_file())
		{
			if (ImGui::Selectable(name.c_str(), selected))
			{
				m_SelectedPath = path;
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (path.extension() == ".scene" && m_Scene)
				{
					m_Scene->LoadSceneBIN(path.string());
					m_Overlay->SelectObject(nullptr);
					break;
				}
				else if (path.extension() == ".tscene" && m_Scene)
				{
					m_Scene->LoadSceneJSON(path.string());
					m_Overlay->SelectObject(nullptr);
					break;
				}
			}
		}
	}
	
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		m_SelectedPath.clear();
	}
}