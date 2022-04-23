#version 460

in vec3 Position;
layout (binding = 0) uniform sampler2D albedoTex;


layout( location = 0 ) out vec4 FragColor;

void main() {

	vec3 albedo = pow(texture(albedoTex, gl_PointCoord).rgb, vec3(2.2));

	FragColor = vec4(albedo,1.0);
}