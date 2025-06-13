#include <ImGui/AssetBrowser.h>

using namespace VulkanRenderer;

AssetBrowser::AssetBrowser(bool open)
	: ImGuiWindow("Asset Browser", open)
{

}

void AssetBrowser::OnRender()
{
	ImGui::Text("This is the asset browser.");
}