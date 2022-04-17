#version 460 core

// code from: https://github.com/metzzo/ezg17-transition
layout (binding = 12) uniform sampler2D volmetricTex;

out vec2 TexCoordsCenter;
out vec2 TexCoordsLeftTop;
out vec2 TexCoordsRightTop;
out vec2 TexCoordsLeftBottom;
out vec2 TexCoordsRightBottom;

vec4 trianglePos(int vertex) {
	float x = -1.0 + float((vertex & 1) << 2);
	float y = -1.0 + float((vertex & 2) << 1);
	return vec4(x, y, 0.0, 1.0);
}

vec2 triangleUV(int vertex) {
	float u = (vertex == 1) ? 2.0 : 0.0; // 0, 2, 0
	float v = (vertex == 2) ? 2.0 : 0.0; // 0, 0, 2
	return vec2(u, v);
}

void main()
{
	vec2 texel_size = 1.0 / textureSize(volmetricTex, 0);
	vec2 uv = triangleUV(gl_VertexID);
	
    TexCoordsCenter = uv;
    TexCoordsLeftTop = uv - 0.5 * texel_size;
    TexCoordsRightTop = TexCoordsLeftTop + vec2(texel_size.x, 0);
	TexCoordsLeftBottom = TexCoordsLeftTop + vec2(0, texel_size.y);
	TexCoordsRightBottom = TexCoordsLeftTop + texel_size;
	
	gl_Position = trianglePos(gl_VertexID);
}