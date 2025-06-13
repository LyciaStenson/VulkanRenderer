#include <InstantiateModelWindow.h>

#include <string>

#include <ModelManager.h>
#include <Scene.h>
#include <Transform.h>

using namespace VulkanRenderer;

inline float Wrap180(float angle)
{
	angle = std::fmod(angle + 180.0f, 360.0f);
	if (angle < 0.0f)
		angle += 360.0f;
	return angle - 180.0f;
}

inline glm::vec3 WrapEuler180(const glm::vec3 angles)
{
	return glm::vec3(Wrap180(angles.x), Wrap180(angles.y), Wrap180(angles.z));
}

inline float RoundDP(float value, int dp)
{
	float factor = (float)std::pow(10.0f, dp);
	return std::round(value * factor) / factor;
}

inline glm::vec3 RoundEulerDP(const glm::vec3& angles, int dp)
{
	return glm::vec3(RoundDP(angles.x, dp), RoundDP(angles.y, dp), RoundDP(angles.z, dp));
}

InstantiateModelWindow::InstantiateModelWindow(ModelManager* modelManager, Scene* scene, bool open)
	: ImGuiWindow("Instantiate Model", open, ImGuiWindowProperties{false, true, ImVec2(400, 600)}), m_ModelManager(modelManager), m_Scene(scene)
{

}

void InstantiateModelWindow::OnRender()
{
	static Transform transform;
	static glm::vec3 cachedEulerDegrees;

	static std::string selectedModel;
	
	ImGui::Text("Instantiate model from name set when loading.");

	for (const auto& [name, model] : m_ModelManager->GetModels())
	{
		if (ImGui::Selectable(name.c_str(), selectedModel == name, 0, ImVec2(0.0f, 30.0f)))
			selectedModel = name;
	}

	ImGui::DragFloat3("Position", &transform.position[0], 0.01f, 0.0f, 0.0f, "%g");
	if (ImGui::DragFloat3("Rotation", &cachedEulerDegrees[0], 0.1f, 0.0f, 0.0f, "%g"))
	{
		cachedEulerDegrees = WrapEuler180(cachedEulerDegrees);
		cachedEulerDegrees = RoundEulerDP(cachedEulerDegrees, 2);
		glm::vec3 eulerRadians = glm::radians(cachedEulerDegrees);

		glm::quat yaw = glm::angleAxis(eulerRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat pitch = glm::angleAxis(eulerRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat roll = glm::angleAxis(eulerRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));

		// Translate back to radians and quaternion for internal memory
		transform.rotation = yaw * pitch * roll;
	}
	ImGui::DragFloat3("Scale", &transform.scale[0], 0.01f, 0.0f, 0.0f, "%g");

	ImGui::BeginDisabled(selectedModel.size() < 1);
	if (ImGui::Button("Instantiate Model"))
	{
		m_Scene->InstantiateModel(selectedModel, transform);
		SetOpen(false);
	}
	ImGui::EndDisabled();
}