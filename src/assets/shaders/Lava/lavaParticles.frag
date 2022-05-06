#version 460

in vec3 Position;
layout (binding = 5) uniform sampler2D albedoTex;


layout( location = 0 ) out vec4 FragColor;

void main() {

	vec3 albedo = vec3(1,0,0);

	FragColor = vec4(albedo,1.0);
}