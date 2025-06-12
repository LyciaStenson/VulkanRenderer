#pragma once

#include <ImGuiWindow.h>

class Inspector : public ImGuiWindow
{
public:
	Inspector(bool open = true);

protected:
	void OnRender() override;
};