#include "Camera.h"

void camera_positioner_player::set_position(const glm::vec3 pos) {
	camera_position_ = pos;
}

void camera_positioner_player::update(double delta_seconds, const glm::vec2& mouse_pos, bool mouse_pressed) {

	const glm::vec2 delta = mouse_pos - last_mouse_pos_;
	last_mouse_pos_ = mouse_pos;

	const auto camera_orientation_before = camera_orientation_;
	auto delta_quat = glm::quat(mouse_speed_ * glm::vec3(delta.y, delta.x, 0.0f));
	camera_orientation_ = glm::normalize(delta_quat * camera_orientation_);
	const glm::mat4 view = get_view_matrix();
	const glm::vec3 dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);

	// Clamp rotation (y-axis)
	const float up_dot = std::abs(glm::dot(up_, dir));
	if (up_dot < 0.99f) {
		camera_orientation_ = glm_look_at(camera_position_, camera_position_ + dir, up_);
	}
	else {
		camera_orientation_ = camera_orientation_before;
	}
}

