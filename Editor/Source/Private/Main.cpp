#include <Core/Engine.h>

#include <Core/Renderer.h>
#include <Core/ModelManager.h>
#include <Core/Scene.h>
#include <Core/Transform.h>
#include <Core/GlfwWindow.h>
#include <Vulkan/DescriptorPool.h>

#include <EditorRenderTarget.h>

#include <filesystem>
#include <fstream>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

using namespace VulkanRenderer;

int main(int argc, char** argv)
{
	Engine engine;
	
	EditorRenderTarget renderTarget(engine.GetRenderer(), engine.GetRenderer()->GetInstance(), engine.GetRenderer()->GetDevice(), engine.GetRenderer()->GetSwapChain(), engine.GetRenderer()->GetRenderPass(), engine.GetGlfwWindow()->Get(), engine.GetScene(), engine.GetModelManager());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	if (!std::filesystem::exists("Assets/Test"))
	{
		std::filesystem::create_directories("Assets/Test");
	}
	
	Transform outTransform;
	outTransform.position = glm::vec3(1.0f, 2.0f, 3.0f);
	{
		std::ofstream os("Assets/Test/Vector.json");
		//std::ofstream os("Assets/Test/Vector.bin", std::ios::binary);
		cereal::JSONOutputArchive outArchive(os);
		//cereal::BinaryOutputArchive outArchive(os);
		outArchive(cereal::make_nvp("vector", outTransform));
	}

	Transform inTransform;
	{
		std::ifstream is("Assets/Test/Vector.json");
		//std::ifstream is("Assets/Test/Vector.bin", std::ios::binary);
		cereal::JSONInputArchive inArchive(is);
		//cereal::BinaryInputArchive inArchive(is);
		inArchive(cereal::make_nvp("vector", inTransform));
	}
	
	std::cout << inTransform.position.x << ", " << inTransform.position.y << ", " << inTransform.position.z << std::endl;

	//Transform cameraTransform{};
	//engine.GetScene()->CreateCamera("Camera", cameraTransform.position, cameraTransform.rotation, cameraTransform.scale);

	//Transform pointLightTransform{};
	//engine.GetScene()->CreatePointLight("Point Light", pointLightTransform.position, pointLightTransform.rotation, pointLightTransform.scale);

	//engine.GetModelManager()->LoadModel("StylisedCar", "Assets/Models/StylisedCar/StylisedCar.glb");

	//Transform stylisedCarTransform{};
	//stylisedCarTransform.position = glm::vec3(0.0f, -1.5f, -10.0f);
	//stylisedCarTransform.scale = glm::vec3(35.0f);
	//engine.GetScene()->InstantiateModel("StylisedCar", stylisedCarTransform);

	engine.Run();
}