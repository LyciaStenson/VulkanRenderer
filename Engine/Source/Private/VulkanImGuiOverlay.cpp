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
	}
	
	void VulkanImGuiOverlay::Draw(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}

	void VulkanImGuiOverlay::DrawSceneGraph(std::vector<std::unique_ptr<SceneObject>>& meshInstances)
	{
		for (auto& meshInstance : meshInstances)
		{
			if (meshInstance->transform.GetParent() == nullptr)
			{
				DrawSceneNode(meshInstance.get());
			}
		}
	}

	void VulkanImGuiOverlay::DrawSceneNode(SceneObject* object)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (object == selectedObject)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (object->transform.GetChildren().empty())
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		
		bool opened = ImGui::TreeNodeEx(object, flags, "%s", object->GetName().c_str());
		
		if (ImGui::IsItemClicked())
			selectedObject = object;

		if (opened && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			for (auto& child : object->transform.GetChildren())
			{
				DrawSceneNode(child->owner);
			}
			ImGui::TreePop();
		}
	}

	void VulkanImGuiOverlay::DrawInspector()
	{
		if (selectedObject)
		{
			ImGui::DragFloat3("Position", &selectedObject->transform.position[0], 0.01f, 0.0f, 0.0f, "%.3f");

			// Translate quaternion rotation to euler angles in degrees for intuitive editing
			glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(selectedObject->transform.rotation));
			if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulerAngles), 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				// Translate back to radians and quaternion for internal memory
				glm::vec3 radians = glm::radians(eulerAngles);
				selectedObject->transform.rotation = glm::quat(radians);
			}
			ImGui::DragFloat3("Scale", &selectedObject->transform.scale[0], 0.01f, 0.0f, 0.0f, "%.3f");

			if (Camera* camera = dynamic_cast<Camera*>(selectedObject))
			{
				ImGui::DragFloat("FOV", &camera->fov);
			}
		}
	}
}