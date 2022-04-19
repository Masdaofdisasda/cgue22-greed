#version 460 core

// code from: https://github.com/metzzo/ezg17-transition
#define UPSAMPLE_DEPTH_THRESHOLD (0.5)

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 lavaLevel;
	mat4 lightViewProj;
	mat4 viewInv;
	mat4 projInv;
	vec4 bloom;
	vec4 deltaTime;
    vec4 normalMap;
    vec4 ssao1;
    vec4 ssao2;
};

in vec2 TexCoordsCenter;
in vec2 TexCoordsLeftTop;
in vec2 TexCoordsRightTop;
in vec2 TexCoordsLeftBottom;
in vec2 TexCoordsRightBottom;

float linear_eye_depth(float depth_value);

layout(binding = 9) uniform sampler2D sceneTex;
layout (binding = 12) uniform sampler2D volmetricTex;
float bloom_treshold = 1.0f;
float near_plane = ssao1.z;
float far_plane = ssao1.w;

void main() {

	vec4 scene_color = texture(sceneTex, TexCoordsCenter);
	
	vec4 volumetric_color;
	
	vec4 volumetric_tex_left_top = texture(volmetricTex, TexCoordsLeftTop);
	vec4 volumetric_tex_right_top = texture(volmetricTex, TexCoordsRightTop);
	vec4 volumetric_tex_left_bottom = texture(volmetricTex, TexCoordsLeftBottom);
	vec4 volumetric_tex_right_bottom = texture(volmetricTex, TexCoordsRightBottom);
	
	vec4 high_res_depth = vec4(linear_eye_depth(scene_color.w));
	vec4 low_res_depth = vec4(
		linear_eye_depth(volumetric_tex_left_top.w),
		linear_eye_depth(volumetric_tex_right_top.w),
		linear_eye_depth(volumetric_tex_left_bottom.w),
		linear_eye_depth(volumetric_tex_right_bottom.w)
	);
	
	vec4 depth_diff = abs(low_res_depth - high_res_depth);
	float accum_diff = dot(depth_diff, vec4(1));
	
	if (accum_diff < UPSAMPLE_DEPTH_THRESHOLD) {
		volumetric_color = texture(volmetricTex, TexCoordsCenter);
	} else {
		float min_depth_diff = depth_diff[0];
		volumetric_color = volumetric_tex_left_top;
		
		if (depth_diff[1] < min_depth_diff) {
			min_depth_diff = depth_diff[1];
			volumetric_color = volumetric_tex_right_top;
		}
		
		if (depth_diff[2] < min_depth_diff) {
			min_depth_diff = depth_diff[2];
			volumetric_color = volumetric_tex_left_bottom;
		}
		
		if (depth_diff[3] < min_depth_diff) {
			min_depth_diff = depth_diff[3];
			volumetric_color = volumetric_tex_right_bottom;
		}
	}
	
	FragColor = volumetric_color + scene_color;
	
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > bloom_treshold) {
		BrightColor = FragColor;
	} else {
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}

float linear_eye_depth(float depth_value) {
	float z = depth_value * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}