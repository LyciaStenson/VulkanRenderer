#include <ImGuiWindow.h>

ImGuiWindow::ImGuiWindow(const std::string& title, bool open)
	: m_Title(title), m_Open(open)
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

	if (ImGui::Begin(m_Title.c_str(), &m_Open))
	{
		OnRender();
	}
	
	ImGui::End();
}