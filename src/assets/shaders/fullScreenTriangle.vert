#version 460 core

layout (location=0) out vec2 uv;

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
	gl_Position = trianglePos(gl_VertexID);
	uv = triangleUV(gl_VertexID);
}
