#include <InspectorWindow.h>

InspectorWindow::InspectorWindow()
	: ImGuiWindow("Inspector")
{

}

void InspectorWindow::OnRender()
{
	ImGui::Text("This is the inspector window.");
}