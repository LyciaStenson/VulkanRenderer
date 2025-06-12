#pragma once

#include <ImGuiWindow.h>

class SceneOutliner : public ImGuiWindow
{
public:
	SceneOutliner(bool open = true);

protected:
	void OnRender() override;
};