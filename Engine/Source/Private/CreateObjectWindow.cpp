#include <CreateObjectWindow.h>

#include <array>

#include <Scene.h>
#include <Transform.h>

using namespace VulkanRenderer;

CreateObjectWindow::CreateObjectWindow(Scene* scene, bool open)
	: ImGuiWindow("Create Object", open, ImGuiWindowProperties{false, true, ImVec2(400, 600)}), m_Scene(scene)
{

}

void CreateObjectWindow::OnRender()
{
	const std::array<const std::string, 3> objectTypes = {"Empty Scene Object", "Mesh Instance", "Camera"};
	static int selectedObjectType = -1;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 15.0f));

	ImGui::Text("Create an empty scene object.");

	ImGui::PopStyleVar(); // Pop spacing

	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
	for (int i = 0; i < objectTypes.size(); ++i)
	{
		if (ImGui::Selectable(objectTypes[i].c_str(), selectedObjectType == i, 0, ImVec2(0.0f, 30.0f)))
			selectedObjectType = i;
	}
	ImGui::PopStyleVar(); // Pop selectable text align

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 15.0f));

	ImGui::Dummy(ImVec2(0.0f, 0.0f));

	ImGui::BeginDisabled(selectedObjectType == -1);
	if (ImGui::Button("Create"))
	{
		Transform transform;

		switch (selectedObjectType)
		{
		case 0:
			m_Scene->CreateSceneObject("Empty Scene Object", transform.position, transform.rotation, transform.scale, nullptr);
			break;
		case 1:
			break;
		case 2:
			Camera* camera = m_Scene->CreateCamera("Camera", transform.position, transform.rotation, transform.scale, nullptr);
			if (!m_Scene->GetMainCamera())
			{
				m_Scene->SetMainCamera(camera);
			}
			break;
		}

		m_Open = false;
		selectedObjectType = -1;
	}
	ImGui::EndDisabled();

	ImGui::PopStyleVar();
}