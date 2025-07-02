#include <SceneOutliner.h>

#include <array>
#include <string>
#include <iostream>

#include <ImGuiOverlay.h>
#include <Core/Scene.h>
#include <Core/PrefabInstance.h>

using namespace VulkanRenderer;

SceneOutliner::SceneOutliner(Scene* scene, VulkanImGuiOverlay* overlay, bool open)
	: ImGuiWindow("Scene Outliner", open, ImGuiWindowProperties{true}), m_Scene(scene), m_Overlay(overlay)
{

}

void SceneOutliner::OnRender()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("New Scene Object"))
		{
			if (ImGui::MenuItem("New Scene Object"))
			{
				m_Overlay->OpenWindow("Create Object Window");
			}
			if (ImGui::MenuItem("Instantiate Model"))
			{
				m_Overlay->OpenWindow("Instantiate Model Window");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Load Model"))
		{
			m_Overlay->OpenWindow("Load Model Window");
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	for (const auto& child : m_Scene->GetRootObject()->GetChildren())
	{
		DrawSceneNode(child.get());
	}
}

void SceneOutliner::DrawSceneNode(SceneObject* object)
{
	if (!object)
		return;
	
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (object == m_Overlay->GetSelectedObject())
		flags |= ImGuiTreeNodeFlags_Selected;

	if (object->GetChildren().empty() || dynamic_cast<PrefabInstance*>(object))
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	bool opened = ImGui::TreeNodeEx(object, flags, "%s", object->GetName().c_str());
	
	if (ImGui::IsItemClicked())
		m_Overlay->SelectObject(object);
	
	if (opened && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
	{
		for (auto& child : object->GetChildren())
		{
			DrawSceneNode(child.get());
		}
		ImGui::TreePop();
	}
}