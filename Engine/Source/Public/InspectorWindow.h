#pragma once

#include <ImGuiWindow.h>

class InspectorWindow : public ImGuiWindow
{
public:
	InspectorWindow();

protected:
	void OnRender() override;
};