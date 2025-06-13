#include <ImGui/Inspector.h>

#include <Vulkan/VulkanImGuiOverlay.h>
#include <Core/SceneObject.h>
#include <Core/Scene.h>
#include <Core/Camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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

Inspector::Inspector(Scene* scene, VulkanImGuiOverlay* overlay, bool open)
	: ImGuiWindow("Inspector", open), m_Scene(scene), m_Overlay(overlay)
{

}

void Inspector::OnRender()
{
	static glm::vec3 cachedEulerDegrees;
	
	SceneObject* selectedObject = m_Overlay->GetSelectedObject();
	if (selectedObject)
	{
		const float xPos = 80.0f;

		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetCursorPosX(xPos);
		ImGui::DragFloat3("##Position", &selectedObject->transform.position[0], 0.01f, 0.0f, 0.0f, "%g");

		static void* lastObject = nullptr;
		if (selectedObject != lastObject)
		{
			cachedEulerDegrees = glm::degrees(glm::eulerAngles(selectedObject->transform.rotation));
			lastObject = selectedObject;
		}

		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::SetCursorPosX(xPos);
		if (ImGui::DragFloat3("##Rotation", &cachedEulerDegrees[0], 0.1f, 0.0f, 0.0f, "%g"))
		{
			cachedEulerDegrees = WrapEuler180(cachedEulerDegrees);
			cachedEulerDegrees = RoundEulerDP(cachedEulerDegrees, 2);
			glm::vec3 eulerRadians = glm::radians(cachedEulerDegrees);

			glm::quat yaw = glm::angleAxis(eulerRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat pitch = glm::angleAxis(eulerRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::quat roll = glm::angleAxis(eulerRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));

			// Translate back to radians and quaternion for internal memory
			selectedObject->transform.rotation = yaw * pitch * roll;
		}

		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::SetCursorPosX(xPos);
		ImGui::DragFloat3("##Scale", &selectedObject->transform.scale[0], 0.01f, 0.0f, 0.0f, "%g");

		if (Camera* camera = dynamic_cast<Camera*>(selectedObject))
		{
			ImGui::Text("FOV");
			ImGui::SameLine();
			ImGui::SetCursorPosX(xPos);
			ImGui::DragFloat("##FOV", &camera->fov, 0.01f, 1.0f, 179.0f, "%g");
		}
	}
}