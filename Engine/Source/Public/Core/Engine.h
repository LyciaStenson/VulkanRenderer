#pragma once

#include <unordered_set>
#include <vector>
#include <memory>

#include <volk.h>

namespace VulkanRenderer
{
	class GlfwWindow;
	class ModelManager;
	class MeshInstance;
	class Scene;
	class Camera;
	class RenderTarget;
	class Renderer;
	
	class Engine
	{
	public:
		Engine();
		~Engine();

		GlfwWindow* GetGlfwWindow() const;
		Renderer* GetRenderer() const;
		Scene* GetScene() const;
		ModelManager* GetModelManager() const;
		
		void Run();

	private:
		std::unique_ptr<GlfwWindow> glfwWindow;

		std::unique_ptr<Renderer> renderer;
		
		std::unique_ptr<Scene> scene;
		std::unique_ptr<ModelManager> modelManager;
	};
}