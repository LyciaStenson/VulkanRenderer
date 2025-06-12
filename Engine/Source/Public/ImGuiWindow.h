#pragma once

#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

class ImGuiWindow
{
public:
	ImGuiWindow(const std::string& title, bool open = true);
	virtual ~ImGuiWindow() = default;

	void SetOpen(bool open);

	void Render();

protected:
	virtual void OnRender() = 0;

	std::string m_Title;
	bool m_Open;
};