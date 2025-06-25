#include <Core/Engine.h>

#include <Core/Renderer.h>
#include <Core/ModelManager.h>
#include <Core/Scene.h>
#include <Core/Transform.h>
#include <Core/GlfwWindow.h>
#include <Vulkan/DescriptorPool.h>

#include <EditorRenderTarget.h>

using namespace VulkanRenderer;

int main(int argc, char** argv)
{
	Engine engine;
	
	EditorRenderTarget renderTarget(engine.GetRenderer(), engine.GetRenderer()->GetInstance(), engine.GetRenderer()->GetDevice(), engine.GetRenderer()->GetSwapChain(), engine.GetRenderer()->GetRenderPass(), engine.GetGlfwWindow()->Get(), engine.GetScene(), engine.GetModelManager());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	Transform cameraTransform{};
	engine.GetScene()->CreateCamera("Camera", cameraTransform.position, cameraTransform.rotation, cameraTransform.scale);

	Transform pointLightTransform{};
	engine.GetScene()->CreatePointLight("Point Light", pointLightTransform.position, pointLightTransform.rotation, pointLightTransform.scale);

	engine.GetModelManager()->LoadModel("StylisedCar", "Assets/Models/StylisedCar/StylisedCar.glb");

	Transform stylisedCarTransform{};
	stylisedCarTransform.position = glm::vec3(0.0f, -1.5f, -10.0f);
	stylisedCarTransform.scale = glm::vec3(35.0f);
	engine.GetScene()->InstantiateModel("StylisedCar", stylisedCarTransform);

	engine.Run();
}