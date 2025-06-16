#version 450

layout(set = 2, binding = 0) uniform MaterialFactorsUBO
{
	vec4 baseColor;
	vec3 metallicRoughness;
} factorsUBO;

layout(set = 2, binding = 1) uniform sampler2D baseColorSampler;
layout(set = 2, binding = 2) uniform sampler2D metallicRoughnessSampler;
layout(set = 2, binding = 3) uniform sampler2D normalSampler;

struct PointLight
{
	vec4 positionRadius;
	vec4 colorIntensity;
};

layout (std430, set = 0, binding = 1) buffer PointLights
{
	PointLight pointLights[];
};

layout (set = 0, binding = 2) uniform PointLightMeta
{
	uint count;
} pointLightMeta;

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
	//outColor = vec4(gammaCorrected, baseColor.a);

	//outColor = vec4(vec3(metallic), 1.0);
	//outColor = vec4(vec3(roughness), 1.0);
	//outColor = vec4(normal, 1.0);

	if (pointLightMeta.count == 0)
	{
		outColor = vec4(0.85f, 0.15f, 0.15f, 1.0f);
	}
	else
	{
		outColor = vec4(0.15f, 0.85f, 0.15f, 1.0f);
	}
}