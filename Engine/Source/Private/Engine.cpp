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

	modelManager = std::make_unique<ModelManager>(device.get(), descriptorSetLayoutManager->GetMeshDescriptorSetLayout());

	opaquePipeline->SetDescriptorPool(descriptorPool->Get());
	transparentPipeline->SetDescriptorPool(descriptorPool->Get());
	
	sync = std::make_unique<VulkanSync>(device->GetLogical());

	GLFWwindow* window = glfwWindow->Get();
	imGuiOverlay = std::make_unique<VulkanImGuiOverlay>(instance.get(), device.get(), swapChain.get(), renderPass.get(), window);
	
	scene = std::make_unique<Scene>(device.get(), modelManager.get(), descriptorSetLayoutManager->GetCameraDescriptorSetLayout(), descriptorPool->Get());
	
	mainCamera = scene->CreateCamera("Camera", glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(), glm::vec3(1.0f), nullptr);

	modelManager->LoadModel("WitchTreeHouse", "Assets/Models/WitchTreehouse/witch_treehouse.glb");

	Transform treehouseTransform;
	treehouseTransform.position = glm::vec3(0.0f, 0.0f, -10.0f);
	scene->InstantiateModel("WitchTreeHouse", treehouseTransform);
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