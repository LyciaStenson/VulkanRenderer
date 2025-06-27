#include <Core/Engine.h>

#include <Core/Renderer.h>
#include <Core/ModelManager.h>
#include <Core/Scene.h>
#include <Core/SceneObject.h>
#include <Core/GlfwWindow.h>
#include <Vulkan/DescriptorPool.h>

#include <EditorRenderTarget.h>

#include <filesystem>
#include <fstream>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

using namespace VulkanRenderer;

int main(int argc, char** argv)
{
	Engine engine;
	
	EditorRenderTarget renderTarget(engine.GetRenderer(), engine.GetRenderer()->GetInstance(), engine.GetRenderer()->GetDevice(), engine.GetRenderer()->GetSwapChain(), engine.GetRenderer()->GetRenderPass(), engine.GetGlfwWindow()->Get(), engine.GetScene(), engine.GetModelManager());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	if (!std::filesystem::exists("Assets/Serialization"))
	{
		std::filesystem::create_directories("Assets/Serialization");
	}
	
	std::vector<std::shared_ptr<SceneObject>> outSceneObjects;
	outSceneObjects.push_back(std::make_shared<SceneObject>("Test Object"));
	outSceneObjects[0]->transform.position = glm::vec3(1.0f, 2.0f, 3.0f);
	{
		std::ofstream os("Assets/Serialization/SceneObjects.json");
		//std::ofstream os("Assets/Serialization/SceneObjects.bin", std::ios::binary);
		cereal::JSONOutputArchive archive(os);
		//cereal::BinaryOutputArchive archive(os);
		archive(cereal::make_nvp("SceneObjects", outSceneObjects));
	}
	
	std::vector<std::shared_ptr<SceneObject>> inSceneObjects;
	{
		std::ifstream is("Assets/Serialization/SceneObjects.json");
		//std::ifstream is("Assets/Serialization/SceneObjects.bin", std::ios::binary);
		cereal::JSONInputArchive archive(is);
		//cereal::BinaryInputArchive archive(is);
		archive(cereal::make_nvp("SceneObjects", inSceneObjects));
	}
	
	std::cout << inSceneObjects[0]->GetName() << ": " << inSceneObjects[0]->transform.position.x << ", " << inSceneObjects[0]->transform.position.y << ", " << inSceneObjects[0]->transform.position.z << std::endl;

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