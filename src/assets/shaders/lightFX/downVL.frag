#version 460 core

// code from: https://github.com/metzzo/ezg17-transition
in vec2 TexCoordsLeftTop;
in vec2 TexCoordsRightTop;
in vec2 TexCoordsLeftBottom;
in vec2 TexCoordsRightBottom;

layout (binding = 12) uniform sampler2D depthMap;

void main() {
	vec4 depth_tex_left_top = texture(depthMap, TexCoordsLeftTop);
	vec4 depth_tex_right_top = texture(depthMap, TexCoordsRightTop);
	vec4 depth_tex_left_bottom = texture(depthMap, TexCoordsLeftBottom);
	vec4 depth_tex_right_bottom = texture(depthMap, TexCoordsRightBottom);
	
	vec4 depth = vec4(
		depth_tex_left_top.x,
		depth_tex_right_top.x,
		depth_tex_left_bottom.x,
		depth_tex_right_bottom.x
	);
	
	float min_depth = min(min(depth.x, depth.y), min(depth.z, depth.w));
	float max_depth = max(max(depth.x, depth.y), max(depth.z, depth.w));
	vec2 position = mod(gl_FragCoord.xy, 2);
	int index = int(position.x + position.y);
	gl_FragDepth = index == 1 ? min_depth : max_depth;
}