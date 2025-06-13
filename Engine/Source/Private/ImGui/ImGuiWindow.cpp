#include <ImGui/ImGuiWindow.h>

using namespace VulkanRenderer;

ImGuiWindow::ImGuiWindow(const std::string& title, bool open, const ImGuiWindowProperties& properties)
	: m_Title(title), m_Open(open), m_HasMenuBar(properties.hasMenuBar), m_FixedSize(properties.fixedSize), m_Size(properties.size)
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
	if (m_FixedSize)
	{
		flags |= ImGuiWindowFlags_NoResize;
		ImGui::SetNextWindowSize(m_Size, ImGuiCond_Once);
	}
	
	if (ImGui::Begin(m_Title.c_str(), &m_Open, flags))
	{
		OnRender();
	}
	
	ImGui::End();
}