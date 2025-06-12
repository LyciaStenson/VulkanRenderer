#include <AssetBrowser.h>

AssetBrowser::AssetBrowser(bool open)
	: ImGuiWindow("Asset Browser", open)
{

}

void AssetBrowser::OnRender()
{
	ImGui::Text("This is the asset browser.");
}