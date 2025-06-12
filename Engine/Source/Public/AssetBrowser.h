#pragma once

#include <ImGuiWindow.h>

class AssetBrowser : public ImGuiWindow
{
public:
	AssetBrowser(bool open = true);

protected:
	void OnRender() override;
};