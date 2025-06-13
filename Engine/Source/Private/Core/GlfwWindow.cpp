#include <Core/GlfwWindow.h>

#include <iostream>

#include <Core/Engine.h>

using namespace VulkanRenderer;

GlfwWindow::GlfwWindow(Engine* engine)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(1280, 720, "Vulkan Renderer", nullptr, nullptr);
	glfwSetWindowUserPointer(window, engine);
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

void GlfwWindow::GetFramebufferSize(int* width, int* height)
{
	glfwGetFramebufferSize(window, width, height);
}

void GlfwWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	Engine* engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	engine->FramebufferResized();
}