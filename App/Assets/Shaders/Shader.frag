#version 450

layout(set = 2, binding = 0) uniform MaterialFactorsUBO
{
	vec4 baseColor;
	vec3 metallicRoughness;
} factorsUBO;

layout(set = 2, binding = 1) uniform sampler2D baseColorSampler;
layout(set = 2, binding = 2) uniform sampler2D metallicRoughnessSampler;
layout(set = 2, binding = 3) uniform sampler2D normalSampler;

layout(location = 0) in vec2 fragBaseColorTexCoord;
layout(location = 1) in vec2 fragMetallicRoughnessTexCoord;
layout(location = 2) in vec2 fragNormalTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	vec4 baseColor = texture(baseColorSampler, fragBaseColorTexCoord) * factorsUBO.baseColor;

	vec4 metallicRoughness = texture(metallicRoughnessSampler, fragMetallicRoughnessTexCoord);
	float metallic = metallicRoughness.b * factorsUBO.metallicRoughness.b;
	float roughness = metallicRoughness.g * factorsUBO.metallicRoughness.g;

	vec3 normal = texture(normalSampler, fragNormalTexCoord).rgb;
	
	vec3 gammaCorrected = pow(baseColor.rgb, vec3(1.0 / 2.2));
	outColor = vec4(gammaCorrected, baseColor.a);

	//outColor = vec4(vec3(metallic), 1.0);
	//outColor = vec4(vec3(roughness), 1.0);
	//outColor = vec4(normal, 1.0);
}