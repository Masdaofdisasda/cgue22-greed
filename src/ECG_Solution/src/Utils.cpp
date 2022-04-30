#include "Utils.h"

glm::vec3 translation_from_transform(const glm::mat4 transform) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);
	return translation;
}

glm::quat rotation_from_transform(const glm::mat4 transform) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);
	return rotation;
}

glm::vec3 scale_from_transform(const glm::mat4 transform) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);
	return scale;
}

glm::mat4 glm_look_at(const glm::vec3 pos, const glm::vec3 target, const glm::vec3 up)
{
	const glm::vec3 zaxis = glm::normalize(pos - target);
	const glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up), zaxis));
	const glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	glm::mat4 translation;
	translation[3][0] = -pos.x;
	translation[3][1] = -pos.y;
	translation[3][2] = -pos.z;
	glm::mat4 rotation;
	rotation[0][0] = xaxis.x;
	rotation[1][0] = xaxis.y;
	rotation[2][0] = xaxis.z;
	rotation[0][1] = yaxis.x;
	rotation[1][1] = yaxis.y;
	rotation[2][1] = yaxis.z;
	rotation[0][2] = zaxis.x;
	rotation[1][2] = zaxis.y;
	rotation[2][2] = zaxis.z;

	return rotation * translation;
}

global_state load_settings()
{
	global_state state;
	std::cout << "reading setting from settings.ini..." << std::endl;
	INIReader reader("../../assets/settings.ini");

	// first param: section [window], second param: property name, third param: default value
	state.width = reader.GetInteger("window", "width", 800);
	state.height = reader.GetInteger("window", "height", 800);
	state.refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	state.fullscreen = reader.GetBoolean("window", "fullscreen", false);
	state.window_title = "Greed";
	state.fov = reader.GetReal("camera", "fov", 60.0f);
	state.znear = 0.1f;
	state.zfar = 1000.0f;

	state.bloom = reader.GetBoolean("image", "bloom", true);
	state.exposure = reader.GetReal("image", "exposure", 0.9f);
	state.max_white = reader.GetReal("image", "maxWhite", 1.07f);
	state.bloom_strength = reader.GetReal("image", "bloomStrength", 0.2f);
	state.adaptation_speed = reader.GetReal("image", "lightAdaption", 0.1f);
	state.ssao = reader.GetBoolean("image", "ssao", true);
	state.scale = reader.GetReal("image", "scale", 1.0f);
	state.bias = reader.GetReal("image", "bias", 0.2f);
	state.radius = reader.GetReal("image", "radius", 0.2f);
	state.att_scale = reader.GetReal("image", "attScale", 1.0f);
	state.dist_scale = reader.GetReal("image", "distScale", 0.5f);
	state.shadow_res = reader.GetInteger("image", "shadowRes", 4);

	return state;
}