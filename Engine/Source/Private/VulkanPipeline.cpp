#include <VulkanPipeline.h>

#include <iostream>
#include <array>

#include <glm/glm.hpp>

#include <Shader.h>
#include <Vertex.h>
#include <Mesh.h>
#include <Camera.h>
#include <VulkanDevice.h>
#include <VulkanSwapChain.h>
#include <VulkanRenderPass.h>
#include <VulkanImGuiOverlay.h>

using namespace VulkanRenderer;

VulkanPipeline::VulkanPipeline(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass)
	: device(device), swapChain(swapChain), renderPass(renderPass)
{
	CreateCameraDescriptorSetLayout();
	CreateMeshDescriptorSetLayout();
	CreateGraphicsPipeline();
}

VulkanPipeline::~VulkanPipeline()
{
	vkDestroyPipeline(device->GetLogical(), pipeline, nullptr);
	vkDestroyPipelineLayout(device->GetLogical(), pipelineLayout, nullptr);

	vkDestroyDescriptorSetLayout(device->GetLogical(), cameraDescriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device->GetLogical(), meshDescriptorSetLayout, nullptr);
}

void VulkanPipeline::SetDescriptorPool(VkDescriptorPool pool)
{
	descriptorPool = pool;
}

void VulkanPipeline::SetImGuiOverlay(VulkanImGuiOverlay* overlay)
{
	imGuiOverlay = overlay;
}

VkDescriptorSetLayout VulkanPipeline::GetCameraDescriptorSetLayout() const
{
	return cameraDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanPipeline::GetMeshDescriptorSetLayout() const
{
	return meshDescriptorSetLayout;
}

void VulkanPipeline::CreateCameraDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(device->GetLogical(), &layoutInfo, nullptr, &cameraDescriptorSetLayout) != VK_SUCCESS)
	{
		std::cerr << "Failed to create camera descriptor set layout" << std::endl;
	}
}

void VulkanPipeline::CreateMeshDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboBinding{};
	uboBinding.binding = 0;
	uboBinding.descriptorCount = 1;
	uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboBinding.pImmutableSamplers = nullptr;
	uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding baseColorBinding{};
	baseColorBinding.binding = 1;
	baseColorBinding.descriptorCount = 1;
	baseColorBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	baseColorBinding.pImmutableSamplers = nullptr;
	baseColorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding roughnessBinding{};
	roughnessBinding.binding = 2;
	roughnessBinding.descriptorCount = 1;
	roughnessBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	roughnessBinding.pImmutableSamplers = nullptr;
	roughnessBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding metallicBinding{};
	metallicBinding.binding = 3;
	metallicBinding.descriptorCount = 1;
	metallicBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	metallicBinding.pImmutableSamplers = nullptr;
	metallicBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	std::array<VkDescriptorSetLayoutBinding, 4> bindings = { uboBinding, baseColorBinding, roughnessBinding, metallicBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device->GetLogical(), &layoutInfo, nullptr, &meshDescriptorSetLayout) != VK_SUCCESS)
	{
		std::cerr << "Failed to create mesh descriptor set layout" << std::endl;
	}
}

void VulkanPipeline::CreateGraphicsPipeline()
{
	Shader vertShader(device->GetLogical(), "Assets/Shaders/Vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	Shader fragShader(device->GetLogical(), "Assets/Shaders/Frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	
	VkPipelineShaderStageCreateInfo shaderStages[] =
	{
		vertShader.GetStageCreateInfo(),
		fragShader.GetStageCreateInfo()
	};

	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateInfo.pDynamicStates = dynamicStates.data();

	auto bindingDescription = Vertex::GetBindingDescription();
	auto attributeDescriptions = Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportStateInfo{};
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
	rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateInfo.lineWidth = 1.0f;
	rasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationStateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationStateInfo.depthBiasClamp = 0.0f;
	rasterizationStateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisamplingStateInfo{};
	multisamplingStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingStateInfo.sampleShadingEnable = VK_FALSE;
	multisamplingStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisamplingStateInfo.minSampleShading = 1.0f;
	multisamplingStateInfo.pSampleMask = nullptr;
	multisamplingStateInfo.alphaToCoverageEnable = VK_FALSE;
	multisamplingStateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.depthTestEnable = VK_TRUE;
	depthStencilInfo.depthWriteEnable = VK_TRUE;
	depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilInfo.minDepthBounds = 0.0f;
	depthStencilInfo.maxDepthBounds = 1.0f;
	depthStencilInfo.stencilTestEnable = VK_FALSE;
	depthStencilInfo.front = {};
	depthStencilInfo.back = {};

	VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachmentState.blendEnable = VK_TRUE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendStateInfo{};
	colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStateInfo.attachmentCount = 1;
	colorBlendStateInfo.pAttachments = &colorBlendAttachmentState;
	colorBlendStateInfo.blendConstants[0] = 0.0f;
	colorBlendStateInfo.blendConstants[1] = 0.0f;
	colorBlendStateInfo.blendConstants[2] = 0.0f;
	colorBlendStateInfo.blendConstants[3] = 0.0f;

	std::array<VkDescriptorSetLayout, 2> descriptorSetLayouts = { cameraDescriptorSetLayout, meshDescriptorSetLayout };
	
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device->GetLogical(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		std::cerr << "Failed to create pipeline layout" << std::endl;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pViewportState = &viewportStateInfo;
	pipelineInfo.pRasterizationState = &rasterizationStateInfo;
	pipelineInfo.pMultisampleState = &multisamplingStateInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.pColorBlendState = &colorBlendStateInfo;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass->Get();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device->GetLogical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		std::cerr << "Failed to create graphics pipeline" << std::endl;
	}
}

void VulkanPipeline::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame, std::vector<std::unique_ptr<Mesh>>& meshes, Camera* camera)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;
	
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		std::cerr << "Failed to begin recording command buffer" << std::endl;
		return;
	}
	
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass->Get();
	renderPassInfo.framebuffer = swapChain->framebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChain->extent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { { 0.0f, 0.0f, 0.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();
	
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChain->extent.width);
	viewport.height = static_cast<float>(swapChain->extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChain->extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	
	// Render each mesh
	for (const std::unique_ptr<Mesh>& mesh : meshes)
	{
		VkBuffer vertexBuffers[] = { mesh->vertexBuffer->Get() };
		VkDeviceSize offsets[] = { 0 };
		
		// Bind vertex and index buffers of mesh
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, mesh->indexBuffer->Get(), 0, VK_INDEX_TYPE_UINT16);
		
		// Bind camera (view & proj matrices) and mesh (model matrix) descriptor sets
		std::array<VkDescriptorSet, 2> descriptorSets = {camera->descriptorSets[currentFrame], mesh->descriptorSets[currentFrame]};
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
		
		// Draw the mesh
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh->GetIndicesSize()), 1, 0, 0, 0);
	}
	
	// If Dear ImGui overlay exists, draw UI representing objects in the scene
	if (imGuiOverlay)
	{
		imGuiOverlay->NewFrame();
		
		// Begin scene UI window
		ImGui::Begin("Scene");

		// Reduce frame padding for drag UI boxes
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));

		if (ImGui::TreeNode("Camera"))
		{
			ImGui::DragFloat3("Position", &camera->transform.position[0], 0.01f, 0.0f, 0.0f, "%.2f");

			// Translate quaternion rotation to euler angles in degrees for intuitive editing
			glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(camera->transform.rotation));
			if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulerAngles), 0.1f, 0.0f, 0.0f, "%.2f"))
			{
				// Translate back to radians and quaternion for internal memory
				glm::vec3 radians = glm::radians(eulerAngles);
				camera->transform.rotation = glm::quat(radians);
			}
			
			ImGui::DragFloat("FOV", &camera->fov, 0.1f, 1.0f, 179.0f, "%.1f");

			ImGui::TreePop();
		}
		
		int i = 1;
		for (const std::unique_ptr<Mesh>& mesh : meshes)
		{
			std::string name = "Mesh " + std::to_string(i);
			if (ImGui::TreeNode(name.c_str()))
			{
				ImGui::DragFloat3("Position", &mesh->transform.position[0], 0.01f, 0.0f, 0.0f, "%.2f");

				// Translate quaternion rotation to euler angles in degrees for intuitive editing
				glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(mesh->transform.rotation));
				if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulerAngles), 0.1f, 0.0f, 0.0f, "%.2f"))
				{
					// Translate back to radians and quaternion for internal memory
					glm::vec3 radians = glm::radians(eulerAngles);
					mesh->transform.rotation = glm::quat(radians);
				}
				ImGui::DragFloat3("Scale", &mesh->transform.scale[0], 0.01f, 0.0f, 0.0f, "%.2f");

				ImGui::TreePop();
			}
			i++;
		}

		if (ImGui::Button("Create mesh"))
		{
			MeshInfo meshInfo;
			meshInfo.vertices =
			{
				{{-0.5f, -0.5f,  0.0f}, {0.0f, 0.0f}},	// Bottom left
				{{ 0.5f, -0.5f,  0.0f}, {1.0f, 0.0f}},	// Bottom right
				{{ 0.5f,  0.5f,  0.0f}, {1.0f, 1.0f}},	// Top right
				{{-0.5f,  0.5f,  0.0f}, {0.0f, 1.0f}}	// Top left
			};
			meshInfo.indices =
			{
				0, 1, 2,
				2, 3, 0
			};
			meshInfo.baseColorPath = "Assets/Textures/BrownRock09_2K_BaseColor.png";
			meshInfo.roughnessPath = "Assets/Textures/BrownRock09_2K_Roughness.png";
			meshInfo.metallicPath = "Assets/Textures/BrownRock09_2K_Metallic.png";
			
			meshes.push_back(std::make_unique<Mesh>(device, GetMeshDescriptorSetLayout(), meshInfo));
			meshes.back()->CreateDescriptorSets(descriptorPool);
			
			meshes.back()->transform.position = {-1.0f, 0.0f, -3.0f};
		}

		// Pop temporary frame padding
		ImGui::PopStyleVar();

		// End scene UI window
		ImGui::End();

		imGuiOverlay->Draw(commandBuffer);
	}
	
	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		std::cerr << "Failed to record command buffer" << std::endl;
	}
}