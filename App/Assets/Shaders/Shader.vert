#version 450

layout(set = 0, binding = 0) uniform CameraUBO
{
	mat4 view;
	mat4 proj;
} camUBO;

layout(set = 1, binding = 0) uniform MeshUBO
{
	mat4 model;
} meshUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inBaseColorTexCoord;
layout(location = 2) in vec2 inMetallicRoughnessTexCoord;
layout(location = 3) in vec2 inNormalTexCoord;

layout(location = 0) out vec2 fragBaseColorTexCoord;
layout(location = 1) out vec2 fragMetallicRoughnessTexCoord;
layout(location = 2) out vec2 fragNormalTexCoord;

void main()
{
	gl_Position = camUBO.proj * camUBO.view * meshUBO.model * vec4(inPosition, 1.0);
	fragBaseColorTexCoord = inBaseColorTexCoord;
	fragMetallicRoughnessTexCoord = inMetallicRoughnessTexCoord;
	fragNormalTexCoord = inNormalTexCoord;
}