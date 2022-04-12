#include "Camera.h"

void CameraPositioner_Player::setPosition(glm::vec3 pos) {
	cameraPosition = pos;
}

void CameraPositioner_Player::update(double deltaSeconds, const glm::vec2& mousePos, bool mousePressed) {

	const glm::vec2 delta = mousePos - lastMousePos;
	lastMousePos = mousePos;

	const glm::quat deltaQuat = glm::quat(mouseSpeed * glm::vec3(delta.y, delta.x, 0.0f));
	cameraOrientation = deltaQuat * cameraOrientation;
	cameraOrientation = glm::normalize(cameraOrientation);

	const glm::mat4 view = getViewMatrix();
	const glm::vec3 dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
	cameraOrientation = lookAt(cameraPosition, cameraPosition + dir, up);
}

glm::mat4 CameraPositioner_Player::lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up)
{
	glm::vec3 zaxis = glm::normalize(pos - target);
	glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up), zaxis));
	glm::vec3 yaxis = glm::cross(zaxis, xaxis);

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