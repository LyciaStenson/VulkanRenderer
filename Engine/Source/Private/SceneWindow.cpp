#include <SceneWindow.h>

SceneWindow::SceneWindow()
	: ImGuiWindow("Scene")
{

}

void SceneWindow::OnRender()
{
	ImGui::Text("This is the scene window.");
}