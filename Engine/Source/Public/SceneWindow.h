#pragma once

#include <ImGuiWindow.h>

class SceneWindow : public ImGuiWindow
{
public:
	SceneWindow();

protected:
	void OnRender() override;
};