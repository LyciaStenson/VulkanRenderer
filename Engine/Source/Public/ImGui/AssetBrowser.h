#pragma once

#include <ImGui/ImGuiWindow.h>

namespace VulkanRenderer
{
	class AssetBrowser : public ImGuiWindow
	{
	public:
		AssetBrowser(bool open = true);

	protected:
		void OnRender() override;
	};
}