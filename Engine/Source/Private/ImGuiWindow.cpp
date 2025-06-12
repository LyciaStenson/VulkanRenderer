#include <ImGuiWindow.h>

ImGuiWindow::ImGuiWindow(const std::string& title, bool open, bool hasMenuBar)
	: m_Title(title), m_Open(open), m_HasMenuBar(hasMenuBar)
{

}

void ImGuiWindow::SetOpen(bool open)
{
	m_Open = open;
}

void ImGuiWindow::Render()
{
	if (!m_Open)
		return;

	ImGuiWindowFlags flags = 0;
	if (m_HasMenuBar)
		flags |= ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin(m_Title.c_str(), &m_Open, flags))
	{
		OnRender();
	}
	
	ImGui::End();
}