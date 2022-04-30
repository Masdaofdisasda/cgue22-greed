#include "Camera.h"

void camera_positioner_player::set_position(glm::vec3 pos) {
	camera_position_ = pos;
}

void camera_positioner_player::update(double delta_seconds, const glm::vec2& mouse_pos, bool mouse_pressed) {

	const glm::vec2 delta = mouse_pos - last_mouse_pos_;
	last_mouse_pos_ = mouse_pos;

	const glm::quat deltaQuat = glm::quat(mouse_speed_ * glm::vec3(delta.y, delta.x, 0.0f));
	camera_orientation_ = deltaQuat * camera_orientation_;
	camera_orientation_ = glm::normalize(camera_orientation_);

	const glm::mat4 view = get_view_matrix();
	const glm::vec3 dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
	camera_orientation_ = look_at(camera_position_, camera_position_ + dir, up_);
}

glm::mat4 camera_positioner_player::look_at(glm::vec3 pos, glm::vec3 target, glm::vec3 up)
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