#include <ImGui/LoadModelWindow.h>

#include <string>

#include <Core/ModelManager.h>

using namespace VulkanRenderer;

LoadModelWindow::LoadModelWindow(ModelManager* modelManager, bool open)
	: ImGuiWindow("Load Model", open, ImGuiWindowProperties{false, true, ImVec2(400, 600)}), m_ModelManager(modelManager)
{

}

void LoadModelWindow::OnRender()
{
	static std::string name;
	static char nameBuffer[1024];

	strncpy_s(nameBuffer, name.c_str(), sizeof(nameBuffer));
	nameBuffer[sizeof(nameBuffer) - 1] = '\0';

	static std::string path;
	static char pathBuffer[1024];

	strncpy_s(pathBuffer, path.c_str(), sizeof(pathBuffer));
	pathBuffer[sizeof(pathBuffer) - 1] = '\0';

	ImGui::Text("Load a .glb format model.");

	if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
	{
		name = std::string(nameBuffer);
	}

	if (ImGui::InputText("Path", pathBuffer, sizeof(pathBuffer)))
	{
		path = std::string(pathBuffer);
	}

	ImGui::BeginDisabled(name.size() < 1 || path.size() < 1);
	if (ImGui::Button("Load Model"))
	{
		m_ModelManager->LoadModel(name, path);
		SetOpen(false);
	}
	ImGui::EndDisabled();
}