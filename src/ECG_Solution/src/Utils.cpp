#include "Utils.h"

glm::vec3 translationFromTransform(glm::mat4 transform) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);
	return translation;
}

glm::quat rotationFromTransform(glm::mat4 transform) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);
	return rotation;
}

glm::vec3 scaleFromTransform(glm::mat4 transform) {
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