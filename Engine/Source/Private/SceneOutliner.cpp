#include <SceneOutliner.h>

#include <array>
#include <string>
#include <iostream>

#include <VulkanImGuiOverlay.h>
#include <Scene.h>

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
				m_Overlay->OpenCreateObjectWindow();
			}
			if (ImGui::MenuItem("Instantiate Model"))
			{
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	for (auto& meshInstance : m_Scene->GetObjectsMutable())
	{
		if (meshInstance->transform.GetParent() == nullptr)
		{
			DrawSceneNode(meshInstance.get());
		}
	}
}

void SceneOutliner::DrawSceneNode(SceneObject* object)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (object == m_Overlay->GetSelectedObject())
		flags |= ImGuiTreeNodeFlags_Selected;

	if (object->transform.GetChildren().empty())
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	bool opened = ImGui::TreeNodeEx(object, flags, "%s", object->GetName().c_str());
	
	if (ImGui::IsItemClicked())
		m_Overlay->SelectObject(object);
	
	if (opened && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
	{
		for (auto& child : object->transform.GetChildren())
		{
			DrawSceneNode(child->owner);
		}
		ImGui::TreePop();
	}
}