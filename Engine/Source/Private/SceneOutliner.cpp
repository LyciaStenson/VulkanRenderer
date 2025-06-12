#include <SceneOutliner.h>

#include <array>
#include <string>
#include <iostream>

SceneOutliner::SceneOutliner(bool open)
	: ImGuiWindow("Scene Outliner", open, true)
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
			}
			if (ImGui::MenuItem("Instantiate Model"))
			{
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("Scene outliner.");
}