#include <Core/Engine.h>

#include <Core/Renderer.h>
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