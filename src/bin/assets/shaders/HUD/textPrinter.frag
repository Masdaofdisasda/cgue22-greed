#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout (binding = 19) uniform sampler2D text;
uniform vec3 color;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, uv).r);
    outColor = vec4(color, 1.0) * sampled;
}  
