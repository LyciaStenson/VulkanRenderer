#version 450

layout(set = 1, binding = 1) uniform sampler2D baseColorSampler;
layout(set = 1, binding = 2) uniform sampler2D roughnessSampler;
layout(set = 1, binding = 3) uniform sampler2D metallicSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	vec4 baseColor = texture(baseColorSampler, fragTexCoord);
	vec4 roughness = texture(roughnessSampler, fragTexCoord);
	vec4 metallic = texture(metallicSampler, fragTexCoord);

	vec3 gammaCorrected = pow(baseColor.rgb, vec3(1.0 / 2.2));
	outColor = vec4(gammaCorrected, baseColor.a);
}