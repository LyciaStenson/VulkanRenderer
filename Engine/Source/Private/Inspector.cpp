#include <Inspector.h>

Inspector::Inspector(bool open)
	: ImGuiWindow("Inspector", open)
{

}

void Inspector::OnRender()
{
	ImGui::Text("This is the inspector.");
}