#pragma once

#include <string>
#include <vector>

#include <volk.h>

namespace VulkanRenderer
{
	class Shader
	{
	public:
		Shader(VkDevice device, const std::string& filePath, VkShaderStageFlagBits stageFlag);
		~Shader();

		const VkPipelineShaderStageCreateInfo& GetStageCreateInfo() const;

	private:
		VkDevice device;
		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo stageCreateInfo{};

		void CreateShaderModule(const std::vector<char>& code);
		static std::vector<char> ReadFile(const std::string& filePath);
	};
}