#pragma once

#include <ImGui/ImGuiWindow.h>

namespace VulkanRenderer
{
	class AboutWindow : public ImGuiWindow
	{
	public:
		AboutWindow(bool open = false);

	protected:
		void OnRender() override;
	};
}