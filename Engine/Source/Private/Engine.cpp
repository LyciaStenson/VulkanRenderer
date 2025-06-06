#include <Engine.h>

#include <iostream>

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
#include <Vertex.h>
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
	renderPass = std::make_unique<VulkanRenderPass>(device.get(), swapChain->imageFormat);
	swapChain->CreateFramebuffers(renderPass->Get());
	descriptorSetLayoutManager = std::make_unique<VulkanDescriptorSetLayoutManager>(device.get());
	opaquePipeline = std::make_unique<VulkanPipeline>(device.get(), swapChain.get(), renderPass.get(), descriptorSetLayoutManager.get());

	camera = std::make_unique<Camera>(device.get(), descriptorSetLayoutManager->GetCameraDescriptorSetLayout());
	camera->transform.position = {0.0f, 0.0f, 0.0f};

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
	
	AddMesh("Brown Rock", meshInfo);
	AddMesh("Red Rock", meshInfo2);

	AddMesh("Glass", meshInfo3);
	AddMesh("Glass", meshInfo3);
	
	opaqueMeshes[0]->transform.position = {-1.0f, 0.0f, -2.0f};
	opaqueMeshes[1]->transform.position = {1.0f, 0.0f, -2.0f};
	
	transparentMeshes[0]->transform.position = {0.0f, 0.0f, -3.5f};
	transparentMeshes[1]->transform.position = {0.0f, 1.0f, -4.0f};

	descriptorPool = std::make_unique<VulkanDescriptorPool>(device.get(), 1000);

	opaquePipeline->SetDescriptorPool(descriptorPool->Get());

	camera->CreateDescriptorSets(descriptorPool->Get());

	for (std::unique_ptr<Mesh>& mesh : opaqueMeshes)
	{
		mesh->CreateDescriptorSets(descriptorPool->Get());
	}
	for (std::unique_ptr<Mesh>& mesh : transparentMeshes)
	{
		mesh->CreateDescriptorSets(descriptorPool->Get());
	}

	sync = std::make_unique<VulkanSync>(device->GetLogical());

	GLFWwindow* window = glfwWindow->Get();
	imGuiOverlay = std::make_unique<VulkanImGuiOverlay>(instance.get(), device.get(), swapChain.get(), renderPass.get(), window);
	opaquePipeline->SetImGuiOverlay(imGuiOverlay.get());
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

	camera->UpdateUniformBuffer(currentFrame, swapChain->extent);

	for (std::unique_ptr<Mesh>& mesh : opaqueMeshes)
	{
		mesh->UpdateUniformBuffer(currentFrame, swapChain->extent);
	}
	for (std::unique_ptr<Mesh>& mesh : transparentMeshes)
	{
		mesh->UpdateUniformBuffer(currentFrame, swapChain->extent);
	}

	opaquePipeline->RecordCommandBuffer(device->commandBuffers[currentFrame], imageIndex, currentFrame, opaqueMeshes, transparentMeshes, camera.get());

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

void Engine::AddMesh(const std::string& name, const MeshInfo& info)
{
	std::string candidateName = name;
	int counter = 1;
	while (meshNames.count(candidateName))
	{
		candidateName = name + std::to_string(counter);
		counter++;
	}
	meshNames.insert(candidateName);
	
	if (info.enableTransparency)
		transparentMeshes.push_back(std::make_unique<Mesh>(device.get(), descriptorSetLayoutManager->GetMeshDescriptorSetLayout(), candidateName, info));
	else
		opaqueMeshes.push_back(std::make_unique<Mesh>(device.get(), descriptorSetLayoutManager->GetMeshDescriptorSetLayout(), candidateName, info));
}