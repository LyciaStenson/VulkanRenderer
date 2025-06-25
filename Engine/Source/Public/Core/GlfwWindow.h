#pragma once

#include <GLFW/glfw3.h>

namespace VulkanRenderer
{
	class Renderer;

	class GlfwWindow
	{
	public:
		GlfwWindow();
		~GlfwWindow();

		GLFWwindow* Get() const;

		void SetRendererPointer(Renderer* renderer);

		void GetFramebufferSize(int* width, int* height);

	private:
		GLFWwindow* window;

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}