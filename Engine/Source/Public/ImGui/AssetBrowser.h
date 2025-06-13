#pragma once

#include <ImGuiWindow.h>

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