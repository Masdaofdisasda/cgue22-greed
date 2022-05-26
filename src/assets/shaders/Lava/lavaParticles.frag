#version 460

in vec3 Position;
//in vec2 UV;
layout(binding = 15) uniform sampler2D particleTex;

layout( location = 0 ) out vec4 FragColor;

void main() {

	//vec4 albedo = texture(particleTex, UV);
	vec4 albedo = texture(particleTex, vec2(0.9,0.5));

	FragColor = albedo;
}