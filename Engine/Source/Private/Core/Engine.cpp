#include <Core/Engine.h>

#include <iostream>
#include <algorithm>

#include <volk.h>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Core/GlfwWindow.h>
#include <Vulkan/Instance.h>
#include <Vulkan/SwapChain.h>
#include <Vulkan/RenderPass.h>
#include <Vulkan/DescriptorSetLayoutManager.h>
#include <Vulkan/Pipeline.h>
#include <Vulkan/DescriptorPool.h>
#include <Vulkan/Sync.h>
#include <Core/ModelManager.h>
#include <Core/MeshInstance.h>
#include <Core/MeshPrimitive.h>
#include <Core/Mesh.h>
#include <Core/Camera.h>
#include <Core/Scene.h>
#include <Core/Vertex.h>
#include <Core/Transform.h>
#include <Vulkan/ImGuiOverlay.h>

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
	float factor = (float)std::pow(10.0f, dp);
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
	
	scene = std::make_unique<Scene>(device.get(), modelManager.get(), descriptorSetLayoutManager->GetGlobalDescriptorSetLayout(), descriptorPool->Get());
	
	imGuiOverlay = std::make_unique<VulkanImGuiOverlay>(instance.get(), device.get(), swapChain.get(), renderPass.get(), glfwWindow->Get(), scene.get(), modelManager.get());
}

Engine::~Engine()
{
	imGuiOverlay.reset();
}

void Engine::FramebufferResized()
{
	framebufferResized = true;
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

	if (scene->GetMainCamera())
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
				const glm::vec3& cameraPosition = scene->GetMainCamera()->transform.position;
				float distA = glm::length(cameraPosition - a->transform.position);
				float distB = glm::length(cameraPosition - b->transform.position);
				return distA > distB;
			});
		
		opaquePipeline->Render(device->commandBuffers[currentFrame], currentFrame, opaqueMeshInstances, scene->GetMainCamera());
		transparentPipeline->Render(device->commandBuffers[currentFrame], currentFrame, transparentMeshInstances, scene->GetMainCamera());
	}
	
	imGuiOverlay->Render(device->commandBuffers[currentFrame]);
	
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