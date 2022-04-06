#version 460 core

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 uv;

void main()
{
    //FragColor = vec4(normalize(vec3(0.710,0.200, 0.180)), 1.0); // same color as lava light source
    FragColor = vec4(1.0,0.0, 0.0, 1.0);
};