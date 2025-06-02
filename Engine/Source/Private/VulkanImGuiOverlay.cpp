#include <VulkanImGuiOverlay.h>

#include <vector>
#include <iostream>

#include <VulkanInstance.h>
#include <VulkanDevice.h>
#include <VulkanSwapChain.h>
#include <VulkanRenderPass.h>
#include <ImGuiDescriptorPool.h>

namespace VulkanRenderer
{
	VulkanImGuiOverlay::VulkanImGuiOverlay(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow)
		: glfwWindow(glfwWindow)
	{
		descriptorPool = std::make_unique<ImGuiDescriptorPool>(device);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& imGuiIO = ImGui::GetIO(); (void)imGuiIO;
		imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		imGuiIO.Fonts->AddFontFromFileTTF("Assets/Fonts/RobotoFlex-Regular.ttf", 16.0f);
		
		ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);

		ImGui_ImplVulkan_InitInfo imGuiInitInfo = {};
		imGuiInitInfo.ApiVersion = VK_API_VERSION_1_3;
		imGuiInitInfo.Instance = instance->Get();
		imGuiInitInfo.PhysicalDevice = device->GetPhysical();
		imGuiInitInfo.Device = device->GetLogical();
		imGuiInitInfo.QueueFamily = device->graphicsQueueFamily;
		imGuiInitInfo.Queue = device->graphicsQueue;
		imGuiInitInfo.DescriptorPool = descriptorPool->Get();
		imGuiInitInfo.RenderPass = renderPass->Get();
		imGuiInitInfo.Subpass = 0;
		imGuiInitInfo.MinImageCount = swapChain->GetMinImageCount();
		imGuiInitInfo.ImageCount = swapChain->GetImageCount();
		imGuiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&imGuiInitInfo);

		ImGui_ImplVulkan_CreateFontsTexture();
	}
	
	VulkanImGuiOverlay::~VulkanImGuiOverlay()
	{
		ImGui_ImplVulkan_DestroyFontsTexture();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void VulkanImGuiOverlay::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiStyle& style = ImGui::GetStyle();
		
		style.TabRounding = 4.0f;
		style.FrameRounding = 4.0f;
		style.WindowRounding = 6.0f;

		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.FramePadding = ImVec2(8.0f, 6.0f);
		style.ItemSpacing = ImVec2(10.0f, 8.0f);

		style.TabBarBorderSize = 0.0f;
		style.WindowBorderSize = 0.0f;

		//ImVec4* colors = style.Colors;

		//colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.85f);
		//colors[ImGuiCol_Text] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
		//colors[ImGuiCol_Button] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
		//colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		//colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
		//colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		//colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
		//colors[ImGuiCol_Header] = colors[ImGuiCol_TitleBg];
		//colors[ImGuiCol_HeaderHovered] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		//colors[ImGuiCol_HeaderActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		
		//colors[ImGuiCol_Tab] = colors[ImGuiCol_TitleBg];
		//colors[ImGuiCol_TabHovered] = colors[ImGuiCol_WindowBg];
		//colors[ImGuiCol_TabSelected] = colors[ImGuiCol_WindowBg];
		//colors[ImGuiCol_TabSelectedOverline] = colors[ImGuiCol_WindowBg];
		//colors[ImGuiCol_TabDimmed] = colors[ImGuiCol_Tab];
		//colors[ImGuiCol_TabDimmedSelected] = colors[ImGuiCol_TabSelected];
		//colors[ImGuiCol_TabDimmedSelectedOverline] = colors[ImGuiCol_TabSelectedOverline];
	}
	
	void VulkanImGuiOverlay::Draw(VkCommandBuffer commandBuffer)
	{
		//ImGui::BeginMainMenuBar();
		//if (ImGui::BeginMenu("File"))
		//{
		//	if (ImGui::MenuItem("Quit"))
		//		glfwSetWindowShouldClose(glfwWindow, true);
		//	ImGui::EndMenu();
		//}
		//ImGui::EndMainMenuBar();
		
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}
}