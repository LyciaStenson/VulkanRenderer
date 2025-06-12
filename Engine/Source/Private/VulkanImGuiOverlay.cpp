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

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

inline float Wrap180(float angle)
{
	angle = std::fmod(angle + 180.0f, 360.0f);
	if (angle < 0.0f)
		angle += 360.0f;
	return angle - 180.0f;
}

inline glm::vec3 WrapEuler180(const glm::vec3 angles)
{
	return glm::vec3(Wrap180(angles.x), Wrap180(angles.y), Wrap180(angles.z));
}

inline float RoundDP(float value, int dp)
{
	float factor = std::pow(10.0f, dp);
	return std::round(value * factor) / factor;
}

inline glm::vec3 RoundEulerDP(const glm::vec3& angles, int dp)
{
	return glm::vec3(RoundDP(angles.x, dp), RoundDP(angles.y, dp), RoundDP(angles.z, dp));
}

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
	
	void VulkanImGuiOverlay::Render(VkCommandBuffer commandBuffer)
	{
		NewFrame();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit"))
					glfwSetWindowShouldClose(glfwWindow, true);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Load Model"))
				{
					//showLoadModel = true;
					//centerLoadModel = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Scene"))
				{
					//showScene = true;
				}
				if (ImGui::MenuItem("Inspector"))
				{
					//showInspector = true;
				}
				if (ImGui::MenuItem("Asset Browser"))
				{
					//showAssetBrowser;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					//showAbout = true;
					//centerAbout = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		sceneWindow.Render();
		inspectorWindow.Render();

		Draw(commandBuffer);
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