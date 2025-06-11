#include <Engine.h>

#include <iostream>
#include <algorithm>

#include <volk.h>

#include <VulkanConfig.h>
#include <VulkanDevice.h>
#include <GlfwWindow.h>
#include <VulkanInstance.h>
#include <VulkanSwapChain.h>
#include <VulkanRenderPass.h>
#include <VulkanDescriptorSetLayoutManager.h>
#include <VulkanPipeline.h>
#include <VulkanDescriptorPool.h>
#include <VulkanSync.h>
#include <ModelManager.h>
#include <MeshInstance.h>
#include <MeshPrimitive.h>
#include <Mesh.h>
#include <Camera.h>
#include <Scene.h>
#include <Vertex.h>
#include <Transform.h>
#include <VulkanImGuiOverlay.h>

using namespace VulkanRenderer;

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

Engine::Engine()
{
	if (volkInitialize() != VK_SUCCESS)
	{
		std::cerr << "Failed to initialize Volk" << std::endl;
	}

	glfwWindow = std::make_unique<GlfwWindow>(this);
	instance = std::make_unique<VulkanInstance>(glfwWindow->Get());
	device = std::make_unique<VulkanDevice>(instance->Get(), instance->GetSurface());
	swapChain = std::make_unique<VulkanSwapChain>(device.get(), instance->GetSurface(), glfwWindow->Get());
	renderPass = std::make_unique<VulkanRenderPass>(device.get(), swapChain.get());
	swapChain->CreateFramebuffers(renderPass->Get());
	descriptorSetLayoutManager = std::make_unique<VulkanDescriptorSetLayoutManager>(device.get());
	opaquePipeline = std::make_unique<VulkanPipeline>(device.get(), renderPass.get(), descriptorSetLayoutManager.get(), PipelineType::Opaque);
	transparentPipeline = std::make_unique<VulkanPipeline>(device.get(), renderPass.get(), descriptorSetLayoutManager.get(), PipelineType::Transparent);

	descriptorPool = std::make_unique<VulkanDescriptorPool>(device.get(), 1000);

	modelManager = std::make_unique<ModelManager>(device.get(), descriptorSetLayoutManager->GetMeshDescriptorSetLayout(), descriptorSetLayoutManager->GetMaterialDescriptorSetLayout(), descriptorPool->Get());

	opaquePipeline->SetDescriptorPool(descriptorPool->Get());
	transparentPipeline->SetDescriptorPool(descriptorPool->Get());
	
	sync = std::make_unique<VulkanSync>(device->GetLogical());

	GLFWwindow* window = glfwWindow->Get();
	imGuiOverlay = std::make_unique<VulkanImGuiOverlay>(instance.get(), device.get(), swapChain.get(), renderPass.get(), window);
	
	scene = std::make_unique<Scene>(device.get(), modelManager.get(), descriptorSetLayoutManager->GetCameraDescriptorSetLayout(), descriptorPool->Get());
}

Engine::~Engine()
{
	imGuiOverlay.reset();
}

void Engine::Run()
{
	while (!glfwWindowShouldClose(glfwWindow->Get()))
	{
		glfwPollEvents();
		DrawFrame();
	}
	vkDeviceWaitIdle(device->GetLogical());
}

void Engine::DrawFrame()
{
	vkWaitForFences(device->GetLogical(), 1, &sync->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device->GetLogical(), swapChain->Get(), UINT64_MAX, sync->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
	{
		framebufferResized = false;
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS)
	{
		std::cerr << "Failed to acquire swap chain image" << std::endl;
		return;
	}
	
	renderPass->Begin(device->commandBuffers[currentFrame], imageIndex);

	if (mainCamera)
	{
		scene->UpdateUniformBuffers(currentFrame, swapChain->extent);

		std::vector<MeshInstance*> opaqueMeshInstances;
		std::vector<MeshInstance*> transparentMeshInstances;

		for (const auto& object : scene->GetObjects())
		{
			if (auto* meshInstance = dynamic_cast<MeshInstance*>(object.get()))
			{
				for (size_t i = 0; i < meshInstance->GetMesh()->GetPrimitiveCount(); ++i)
				{
					if (meshInstance->GetMesh()->GetPrimitive(i)->GetTransparencyEnabled())
						transparentMeshInstances.push_back(meshInstance);
					else
						opaqueMeshInstances.push_back(meshInstance);
				}
			}
		}

		std::sort(transparentMeshInstances.begin(), transparentMeshInstances.end(),
			[&](MeshInstance* a, MeshInstance* b)
			{
				float distA = glm::length(mainCamera->transform.position - a->transform.position);
				float distB = glm::length(mainCamera->transform.position - b->transform.position);
				return distA > distB;
			});
		
		opaquePipeline->Render(device->commandBuffers[currentFrame], currentFrame, opaqueMeshInstances, mainCamera);
		transparentPipeline->Render(device->commandBuffers[currentFrame], currentFrame, transparentMeshInstances, mainCamera);
	}
	// If Dear ImGui overlay exists, draw UI representing objects in the scene
	if (imGuiOverlay)
	{
		imGuiOverlay->NewFrame();
		
		const std::array<const std::string, 3> objectTypes = {"Empty Scene Object", "Mesh Instance", "Camera"};
		static int selectedObjectType = -1;
		
		static bool showLoadModel = false;
		static bool centerLoadModel = false;

		static bool showAbout = false;
		static bool centerAbout = false;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Load Model"))
				{
					showLoadModel = true;
					centerLoadModel = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					showAbout = true;
					centerAbout = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (showLoadModel)
		{
			if (centerLoadModel)
			{
				ImVec2 windowSize = ImVec2(600, 400);
				ImVec2 displaySize = ImGui::GetIO().DisplaySize;
				ImVec2 windowPos = ImVec2((displaySize.x - windowSize.x) * 0.5f, (displaySize.y - windowSize.y) * 0.5f);

				ImGui::SetNextWindowPos(windowPos);
				ImGui::SetNextWindowSize(windowSize);

				centerLoadModel = false;
			}

			static std::string name;
			static char nameBuffer[1024];

			std::strncpy(nameBuffer, name.c_str(), sizeof(nameBuffer));
			nameBuffer[sizeof(nameBuffer) - 1] = '\0';

			static std::string path;
			static char pathBuffer[1024];

			std::strncpy(pathBuffer, path.c_str(), sizeof(pathBuffer));
			pathBuffer[sizeof(pathBuffer) - 1] = '\0';

			ImGui::Begin("Load Model", &showLoadModel, ImGuiWindowFlags_NoResize);
			ImGui::Text("Load a .glb format model.");

			if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
			{
				name = std::string(nameBuffer);
			}

			if (ImGui::InputText("Path", pathBuffer, sizeof(pathBuffer)))
			{
				path = std::string(pathBuffer);
			}

			ImGui::BeginDisabled(name.size() < 1 || path.size() < 1);
			if (ImGui::Button("Load Model"))
			{
				modelManager->LoadModel(name, path);
			}
			ImGui::EndDisabled();

			ImGui::End();
		}

		if (showAbout)
		{
			if (centerAbout)
			{
				ImVec2 windowSize = ImVec2(200, 150);
				ImVec2 displaySize = ImGui::GetIO().DisplaySize;
				ImVec2 windowPos = ImVec2((displaySize.x - windowSize.x) * 0.5f, (displaySize.y - windowSize.y) * 0.5f);

				ImGui::SetNextWindowPos(windowPos);
				ImGui::SetNextWindowSize(windowSize);

				centerAbout = false;
			}

			ImGui::Begin("About Vulkan Renderer", &showAbout, ImGuiWindowFlags_NoResize);
			ImGui::Text("A simple Vulkan Renderer.");
			ImGui::End();
		}

		static bool showCreateWindow = false;
		static bool centerCreateWindow = false;

		static bool showInstantiateModel = false;
		static bool centerInstantiateModel = false;

		// Begin scene UI window
		ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("New Scene Object"))
				{
					if (ImGui::MenuItem("New Scene Object"))
					{
						showCreateWindow = true;
						centerCreateWindow = true;
					}
					if (ImGui::MenuItem("Instantiate Model"))
					{
						showInstantiateModel = true;
						centerInstantiateModel = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			
			if (showCreateWindow)
			{
				if (centerCreateWindow)
				{
					ImVec2 windowSize = ImVec2(500, 600);
					ImVec2 displaySize = ImGui::GetIO().DisplaySize;
					ImVec2 windowPos = ImVec2((displaySize.x - windowSize.x) * 0.5f, (displaySize.y - windowSize.y) * 0.5f);

					ImGui::SetNextWindowPos(windowPos);
					ImGui::SetNextWindowSize(windowSize);

					centerCreateWindow = false;
				}
				
				ImGui::Begin("Create Scene Object", &showCreateWindow, ImGuiWindowFlags_NoResize);
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 15.0f));

					ImGui::Text("Create an empty scene object.");

					ImGui::PopStyleVar(); // Pop spacing

					ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
					for (int i = 0; i < objectTypes.size(); ++i)
					{
						if (ImGui::Selectable(objectTypes[i].c_str(), selectedObjectType == i, 0, ImVec2(0.0f, 30.0f)))
							selectedObjectType = i;
					}
					ImGui::PopStyleVar(); // Pop selectable text align

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 15.0f));

					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					
					ImGui::BeginDisabled(selectedObjectType == -1);
					if (ImGui::Button("Create"))
					{
						Transform transform;

						switch (selectedObjectType)
						{
						case 0:
							scene->CreateSceneObject("Empty Scene Object", transform.position, transform.rotation, transform.scale, nullptr);
							break;
						case 1:
							break;
						case 2:
							Camera* camera = scene->CreateCamera("Camera", transform.position, transform.rotation, transform.scale, nullptr);
							if (!mainCamera)
							{
								mainCamera = camera;
							}
							break;
						}

						showCreateWindow = false;
						selectedObjectType = -1;
					}
					ImGui::EndDisabled();
					
					ImGui::PopStyleVar();
				}
				ImGui::End();
			}

			if (showInstantiateModel)
			{
				if (centerInstantiateModel)
				{
					ImVec2 windowSize = ImVec2(500, 600);
					ImVec2 displaySize = ImGui::GetIO().DisplaySize;
					ImVec2 windowPos = ImVec2((displaySize.x - windowSize.x) * 0.5f, (displaySize.y - windowSize.y) * 0.5f);

					ImGui::SetNextWindowPos(windowPos);
					ImGui::SetNextWindowSize(windowSize);

					centerInstantiateModel = false;
				}

				static Transform transform;
				static glm::vec3 cachedEulerDegrees;

				static std::string selectedModel;
				
				ImGui::Begin("Instantiate Model", &showInstantiateModel, ImGuiWindowFlags_NoResize);
				ImGui::Text("Instantiate model from name set when loading.");

				for (const auto& [name, model] : modelManager->GetModels())
				{
					if (ImGui::Selectable(name.c_str(), selectedModel == name, 0, ImVec2(0.0f, 30.0f)))
						selectedModel = name;
				}
				
				ImGui::DragFloat3("Position", &transform.position[0], 0.01f, 0.0f, 0.0f, "%g");
				if (ImGui::DragFloat3("Rotation", &cachedEulerDegrees[0], 0.1f, 0.0f, 0.0f, "%g"))
				{
					cachedEulerDegrees = WrapEuler180(cachedEulerDegrees);
					cachedEulerDegrees = RoundEulerDP(cachedEulerDegrees, 2);
					glm::vec3 eulerRadians = glm::radians(cachedEulerDegrees);

					glm::quat yaw = glm::angleAxis(eulerRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
					glm::quat pitch = glm::angleAxis(eulerRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
					glm::quat roll = glm::angleAxis(eulerRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));

					// Translate back to radians and quaternion for internal memory
					transform.rotation = yaw * pitch * roll;
				}
				ImGui::DragFloat3("Scale", &transform.scale[0], 0.01f, 0.0f, 0.0f, "%g");

				ImGui::BeginDisabled(selectedModel.size() < 1);
				if (ImGui::Button("Instantiate Model"))
				{
					scene->InstantiateModel(selectedModel, transform);
					showInstantiateModel = false;
				}
				ImGui::EndDisabled();

				ImGui::End();
			}
			
			imGuiOverlay->DrawSceneGraph(scene->GetObjectsMutable());
		}
		// End scene UI window
		ImGui::End();

		ImGui::Begin("Inspector");
		{
			imGuiOverlay->DrawInspector();
		}
		ImGui::End();

		imGuiOverlay->Draw(device->commandBuffers[currentFrame]);
	}

	renderPass->End(device->commandBuffers[currentFrame]);

	vkResetFences(device->GetLogical(), 1, &sync->inFlightFences[currentFrame]);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {sync->imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device->commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = {sync->renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, sync->inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		std::cerr << "Failed to submit draw command buffer" << std::endl;
		return;
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapChain->Get()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(device->presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
	{
		framebufferResized = false;
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		std::cerr << "Failed to present swap chain image" << std::endl;
	}

	currentFrame = (currentFrame + 1) % VulkanConfig::MAX_FRAMES_IN_FLIGHT;
}

void Engine::RecreateSwapChain()
{
	int width = 0, height = 0;
	glfwWindow->GetFramebufferSize(&width, &height);

	while (width == 0 || height == 0)
	{
		glfwWindow->GetFramebufferSize(&width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device->GetLogical());

	swapChain->CleanupSwapChain();
	sync->CleanupSyncObjects();

	swapChain->CreateSwapChain();
	swapChain->CreateDepthResources();
	swapChain->CreateFramebuffers(renderPass->Get());
	sync->CreateSyncObjects();
}