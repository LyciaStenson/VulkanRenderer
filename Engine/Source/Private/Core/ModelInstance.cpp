#include <Core/ModelInstance.h>

using namespace VulkanRenderer;

ModelInstance::ModelInstance(const std::string& name, const std::string& modelPath)
	: SceneObject(name), modelPath(modelPath)
{

}

ModelInstance::~ModelInstance()
{

}

const std::string& ModelInstance::GetModelPath() const
{
	return modelPath;
}