#include <ImGui/AboutWindow.h>

using namespace VulkanRenderer;

AboutWindow::AboutWindow(bool open)
	: ImGuiWindow("About", open)
{

}

void AboutWindow::OnRender()
{
	ImGui::Text("A simple 3D renderer using C++ and Vulkan.");
}