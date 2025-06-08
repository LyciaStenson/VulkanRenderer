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
#include <MeshManager.h>
#include <MeshInstance.h>
#include <Scene.h>
#include <Vertex.h>
#include <Transform.h>
#include <VulkanImGuiOverlay.h>

using namespace VulkanRenderer;

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

	meshManager = std::make_unique<MeshManager>(device.get(), descriptorSetLayoutManager->GetMeshDescriptorSetLayout());

	opaquePipeline->SetDescriptorPool(descriptorPool->Get());
	transparentPipeline->SetDescriptorPool(descriptorPool->Get());
	
	sync = std::make_unique<VulkanSync>(device->GetLogical());

	GLFWwindow* window = glfwWindow->Get();
	imGuiOverlay = std::make_unique<VulkanImGuiOverlay>(instance.get(), device.get(), swapChain.get(), renderPass.get(), window);
	
	scene = std::make_unique<Scene>(device.get(), meshManager.get(), descriptorSetLayoutManager->GetCameraDescriptorSetLayout(), descriptorPool->Get());

	Transform cameraTransform;
	cameraTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	mainCamera = scene->CreateCamera("Camera", cameraTransform);

	// MeshInfo holds the vertices, indices, and texture paths to be passed to Mesh constructor
	MeshInfo meshInfo;
	meshInfo.vertices =
	{
		{{-0.5f, -0.5f,  0.0f}, {0.0f, 0.0f}},	// Bottom left
		{{ 0.5f, -0.5f,  0.0f}, {1.0f, 0.0f}},	// Bottom right
		{{ 0.5f,  0.5f,  0.0f}, {1.0f, 1.0f}},	// Top right
		{{-0.5f,  0.5f,  0.0f}, {0.0f, 1.0f}}	// Top left
	};
	meshInfo.indices =
	{
		0, 1, 2,
		2, 3, 0
	};
	meshInfo.baseColorPath = "Assets/Textures/BrownRock09_2K_BaseColor.png";
	meshInfo.roughnessPath = "Assets/Textures/BrownRock09_2K_Roughness.png";
	meshInfo.metallicPath = "Assets/Textures/BrownRock09_2K_Metallic.png";
	
	// Reuse vertices and indices
	MeshInfo meshInfo2;
	meshInfo2.vertices = meshInfo.vertices;
	meshInfo2.indices = meshInfo.indices;
	meshInfo2.baseColorPath = "Assets/Textures/RedRock05_2K_BaseColor.png";
	meshInfo2.roughnessPath = "Assets/Textures/RedRock05_2K_Roughness.png";
	meshInfo2.metallicPath = "Assets/Textures/RedRock05_2K_Metallic.png";
	
	MeshInfo meshInfo3;
	meshInfo3.vertices = meshInfo.vertices;
	meshInfo3.indices = meshInfo.indices;
	meshInfo3.baseColorPath = "Assets/Textures/Glass_Vintage_001_basecolor.png";
	meshInfo3.roughnessPath = "Assets/Textures/Glass_Vintage_001_roughness.jpg";
	meshInfo3.metallicPath = "Assets/Textures/Glass_Vintage_001_metallic.png";
	meshInfo3.enableTransparency = true;
	
	meshManager->LoadMesh("Brown Rock", meshInfo);
	meshManager->LoadMesh("Red Rock", meshInfo2);
	meshManager->LoadMesh("Glass", meshInfo3);
	
	Transform brownRockTransform;
	brownRockTransform.position = glm::vec3(-1.0f, 0.0f, -2.0f);
	MeshInstance* brownRock = scene->CreateMeshInstance("Brown Rock", brownRockTransform);
	
	Transform redRockTransform;
	redRockTransform.position = glm::vec3(1.0f, 0.0f, -2.0f);
	MeshInstance* redRock = scene->CreateMeshInstance("Red Rock", redRockTransform);
	
	Transform glassParentTransform;
	glassParentTransform.position = glm::vec3(0.0f, 1.0f, -3.0f);
	SceneObject* glassParent = scene->CreateSceneObject("GlassParent", glassParentTransform);

	Transform glassTransform;
	glassTransform.position = glm::vec3(-1.0f, 0.0f, 0.0f);
	MeshInstance* glass = scene->CreateMeshInstance("Glass", glassTransform);
	glass->transform.SetParent(&glassParent->transform);
	
	Transform glass2Transform;
	glass2Transform.position = glm::vec3(1.0f, 0.0f, 0.0f);
	MeshInstance* glass2 = scene->CreateMeshInstance("Glass", glass2Transform);
	glass2->transform.SetParent(&glassParent->transform);
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
	
	scene->UpdateUniformBuffers(currentFrame, swapChain->extent);

	renderPass->Begin(device->commandBuffers[currentFrame], imageIndex);
	
	std::vector<MeshInstance*> opaqueMeshInstances;
	std::vector<MeshInstance*> transparentMeshInstances;

	for (const auto& object : scene->GetObjects())
	{
		if (auto* meshInstance = dynamic_cast<MeshInstance*>(object.get()))
		{
			if (meshInstance->GetMesh()->GetTransparencyEnabled())
				transparentMeshInstances.push_back(meshInstance);
			else
				opaqueMeshInstances.push_back(meshInstance);
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
	
	// If Dear ImGui overlay exists, draw UI representing objects in the scene
	if (imGuiOverlay)
	{
		imGuiOverlay->NewFrame();

		// Begin scene UI window
		ImGui::Begin("Scene");
		{
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