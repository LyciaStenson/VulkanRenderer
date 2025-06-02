#pragma once

#include <GLFW/glfw3.h>

namespace VulkanRenderer
{
	class Engine;

	class GlfwWindow
	{
	public:
		GlfwWindow(Engine* engine);
		~GlfwWindow();

		GLFWwindow* Get() const;

		void GetFramebufferSize(int* width, int* height);

	private:
		GLFWwindow* window;

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}