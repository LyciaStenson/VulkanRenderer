#include <VulkanImGuiOverlay.h>

#include <vector>
#include <iostream>

#include <VulkanInstance.h>
#include <VulkanDevice.h>
#include <VulkanSwapChain.h>
#include <VulkanRenderPass.h>
#include <ImGuiDescriptorPool.h>
#include <MeshInstance.h>
#include <Camera.h>
#include <Scene.h>
#include <SceneOutliner.h>
#include <CreateObjectWindow.h>
#include <Inspector.h>
#include <AssetBrowser.h>
#include <AboutWindow.h>

namespace VulkanRenderer
{
	VulkanImGuiOverlay::VulkanImGuiOverlay(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene)
		: m_Window(glfwWindow)
	{
		m_DescriptorPool = std::make_unique<ImGuiDescriptorPool>(device);

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
		imGuiInitInfo.DescriptorPool = m_DescriptorPool->Get();
		imGuiInitInfo.RenderPass = renderPass->Get();
		imGuiInitInfo.Subpass = 0;
		imGuiInitInfo.MinImageCount = swapChain->GetMinImageCount();
		imGuiInitInfo.ImageCount = swapChain->GetImageCount();
		imGuiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&imGuiInitInfo);

		ImGui_ImplVulkan_CreateFontsTexture();

		m_Windows["Scene Outliner"] = std::make_unique<SceneOutliner>(scene, this);
		m_Windows["Create Object Window"] = std::make_unique<CreateObjectWindow>(scene);
		m_Windows["Inspector"] = std::make_unique<Inspector>(scene, this);
		m_Windows["Asset Browser"] = std::make_unique<AssetBrowser>();
		m_Windows["About"] = std::make_unique<AboutWindow>();
	}
	
	VulkanImGuiOverlay::~VulkanImGuiOverlay()
	{
		ImGui_ImplVulkan_DestroyFontsTexture();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	SceneObject* VulkanImGuiOverlay::GetSelectedObject() const
	{
		return m_SelectedObject;
	}

	void VulkanImGuiOverlay::SelectObject(SceneObject* object)
	{
		m_SelectedObject = object;
	}
	
	void VulkanImGuiOverlay::Render(VkCommandBuffer commandBuffer)
	{
		NewFrame();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit"))
					glfwSetWindowShouldClose(m_Window, true);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Scene Outliner"))
				{
					if (m_Windows.count("Scene Outliner"))
						m_Windows["Scene Outliner"]->SetOpen(true);
				}
				if (ImGui::MenuItem("Inspector"))
				{
					if (m_Windows.count("Inspector"))
						m_Windows["Inspector"]->SetOpen(true);
				}
				if (ImGui::MenuItem("Asset Browser"))
				{
					if (m_Windows.count("Asset Browser"))
						m_Windows["Asset Browser"]->SetOpen(true);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					if (m_Windows.count("About"))
						m_Windows["About"]->SetOpen(true);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		for (auto& [_, window] : m_Windows)
		{
			window->Render();
		}
		
		Draw(commandBuffer);
	}

	void VulkanImGuiOverlay::OpenCreateObjectWindow()
	{
		if (m_Windows.count("Create Object Window"))
			m_Windows["Create Object Window"]->SetOpen(true);
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
	}
	
	void VulkanImGuiOverlay::Draw(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}

	//void VulkanImGuiOverlay::DrawSceneGraph(std::vector<std::unique_ptr<SceneObject>>& meshInstances)
	//{
	//	for (auto& meshInstance : meshInstances)
	//	{
	//		if (meshInstance->transform.GetParent() == nullptr)
	//		{
	//			DrawSceneNode(meshInstance.get());
	//		}
	//	}
	//}

	//void VulkanImGuiOverlay::DrawSceneNode(SceneObject* object)
	//{
	//	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	//	if (object == selectedObject)
	//		flags |= ImGuiTreeNodeFlags_Selected;

	//	if (object->transform.GetChildren().empty())
	//		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	//	
	//	bool opened = ImGui::TreeNodeEx(object, flags, "%s", object->GetName().c_str());
	//	
	//	if (ImGui::IsItemClicked())
	//		selectedObject = object;

	//	if (opened && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
	//	{
	//		for (auto& child : object->transform.GetChildren())
	//		{
	//			DrawSceneNode(child->owner);
	//		}
	//		ImGui::TreePop();
	//	}
	//}

	//void VulkanImGuiOverlay::DrawInspector(bool* show)
	//{
	//	if (*show)
	//	{
	//		static glm::vec3 cachedEulerDegrees;
	//		
	//		ImGui::Begin("Inspector");
	//		
	//		if (selectedObject)
	//		{
	//			const float xPos = 80.0f;

	//			ImGui::Text("Position");
	//			ImGui::SameLine();
	//			ImGui::SetCursorPosX(xPos);
	//			ImGui::DragFloat3("##Position", &selectedObject->transform.position[0], 0.01f, 0.0f, 0.0f, "%g");

	//			static void* lastObject = nullptr;
	//			if (selectedObject != lastObject)
	//			{
	//				cachedEulerDegrees = glm::degrees(glm::eulerAngles(selectedObject->transform.rotation));
	//				lastObject = selectedObject;
	//			}

	//			ImGui::Text("Rotation");
	//			ImGui::SameLine();
	//			ImGui::SetCursorPosX(xPos);
	//			if (ImGui::DragFloat3("##Rotation", &cachedEulerDegrees[0], 0.1f, 0.0f, 0.0f, "%g"))
	//			{
	//				cachedEulerDegrees = WrapEuler180(cachedEulerDegrees);
	//				cachedEulerDegrees = RoundEulerDP(cachedEulerDegrees, 2);
	//				glm::vec3 eulerRadians = glm::radians(cachedEulerDegrees);

	//				glm::quat yaw = glm::angleAxis(eulerRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
	//				glm::quat pitch = glm::angleAxis(eulerRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
	//				glm::quat roll = glm::angleAxis(eulerRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));

	//				// Translate back to radians and quaternion for internal memory
	//				selectedObject->transform.rotation = yaw * pitch * roll;
	//			}

	//			ImGui::Text("Scale");
	//			ImGui::SameLine();
	//			ImGui::SetCursorPosX(xPos);
	//			ImGui::DragFloat3("##Scale", &selectedObject->transform.scale[0], 0.01f, 0.0f, 0.0f, "%g");

	//			if (Camera* camera = dynamic_cast<Camera*>(selectedObject))
	//			{
	//				ImGui::Text("FOV");
	//				ImGui::SameLine();
	//				ImGui::SetCursorPosX(xPos);
	//				ImGui::DragFloat("##FOV", &camera->fov, 0.01f, 1.0f, 179.0f, "%g");
	//			}
	//		}
	//		ImGui::End();
	//	}
	//}

	//void VulkanImGuiOverlay::DrawAssetBrowser(bool* show)
	//{
	//	ImGui::Begin("Asset Browser", show);
	//	ImGui::End();
	//}
}