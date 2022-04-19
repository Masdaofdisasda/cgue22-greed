#version 460 core

// code from: https://github.com/metzzo/ezg17-transition
#define GAUSS_BLUR_DEVIATION (1.5/2.0) 
#define PI (3.1415927)
#define HALF_RES_BLUR_KERNEL_SIZE (3)
#define BLUR_DEPTH_FACTOR 0.5
 
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D volumetric_tex;
uniform bool is_vert;
uniform float near_plane;
uniform float far_plane;

vec4 bilateral_blur(vec2 direction, vec2 texel_size, int kernel_radius);

void main() {
	vec4 color;
	vec2 texel_size = 1.0 / textureSize(volumetric_tex, 0); // gets size of single texel
	
	if (is_vert) {
		color = bilateral_blur(vec2(1, 0), texel_size, HALF_RES_BLUR_KERNEL_SIZE);
	} else {
		color = bilateral_blur(vec2(0, 1), texel_size, HALF_RES_BLUR_KERNEL_SIZE);
	}
	
	FragColor = color;	
}

float linear_eye_depth(float depth_value) {
	float z = depth_value * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

// TODO precompute
float gaussian_weight(float offset, float deviation) {
	float weight = 1.0f / sqrt(2.0f * PI * deviation * deviation);
	weight *= exp(-(offset * offset) / (2.0f * deviation * deviation));
	return weight;
}


vec4 bilateral_blur(vec2 direction, vec2 texel_size, int kernel_radius) {
	float deviation = kernel_radius / GAUSS_BLUR_DEVIATION;

	vec4 center_color = texture(volumetric_tex, TexCoords);
	vec3 color = center_color.xyz;
	
	float center_depth = linear_eye_depth(center_color.w);

	float weight_sum = 0;

	float weight = gaussian_weight(0, deviation);
	
	color *= weight;
	weight_sum += weight;
				
	for (int i = -kernel_radius; i < 0; i += 1) {
		vec2 position = TexCoords + (direction * i) * texel_size;
		vec4 sample_color = texture(volumetric_tex, position);
		float sample_depth = linear_eye_depth(sample_color.w);

		float depth_diff = abs(center_depth - sample_depth);
		float dfactor = depth_diff * BLUR_DEPTH_FACTOR;
		float w = exp(-(dfactor * dfactor));

		weight = gaussian_weight(i, deviation) * w;

		color += weight * sample_color.xyz;
		weight_sum += weight;
	}

	for (int i = 1; i <= kernel_radius; i += 1) {
		vec2 position = TexCoords + (direction * i) * texel_size;
		vec4 sample_color = texture(volumetric_tex, position);
		float sample_depth = linear_eye_depth(sample_color.w);

		float depth_diff = abs(center_depth - sample_depth);
		float dfactor = depth_diff * BLUR_DEPTH_FACTOR;
		float w = exp(-(dfactor * dfactor));
		
		weight = gaussian_weight(i, deviation) * w;

		color += weight * sample_color.xyz;
		weight_sum += weight;
	}

	color /= weight_sum;
	return vec4(color, 0.0);
}

		
		