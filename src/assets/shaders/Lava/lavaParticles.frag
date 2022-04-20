#version 460

in vec3 Position;

uniform vec4 col1 = vec4(0.95, 0.62, 0.02, 1.0)*0.05; // yellow
uniform vec4 col2 = vec4(0.95, 0.53, 0.02, 1.0)*0.05;
uniform vec4 col3 = vec4(0.95, 0.38, 0.06, 1.0)*0.05;
uniform vec4 col4 = vec4(0.75, 0.23, 0.06, 1.0)*0.05;
uniform vec4 col5 = vec4(0.55, 0.18, 0.11, 1.0)*0.05; // red

layout( location = 0 ) out vec4 FragColor;

void main() {
	vec4 colors[5] = {
	col1, col2, col3, col4, col5
	};

	FragColor = colors[4];
}