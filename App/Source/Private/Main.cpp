#include <Core/Engine.h>

#include <Core/Renderer.h>
#include <Core/ModelManager.h>
#include <Core/Scene.h>
#include <Core/Transform.h>
#include <Vulkan/DescriptorPool.h>

#include <AppRenderTarget.h>

using namespace VulkanRenderer;

int main(int argc, char** argv)
{
	Engine engine;
	
	AppRenderTarget renderTarget(engine.GetRenderer());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	engine.Run();
}