#include <Core/GlfwWindow.h>

#include <iostream>

#include <Core/Renderer.h>

using namespace VulkanRenderer;

GlfwWindow::GlfwWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(1280, 720, "Vulkan Renderer", nullptr, nullptr);
	glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
}

GlfwWindow::~GlfwWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWwindow* GlfwWindow::Get() const
{
	return window;
}

void GlfwWindow::SetRendererPointer(Renderer* renderer)
{
	glfwSetWindowUserPointer(window, renderer);
}

void GlfwWindow::GetFramebufferSize(int* width, int* height)
{
	glfwGetFramebufferSize(window, width, height);
}

void GlfwWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	Renderer* renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	renderer->FramebufferResized();
}