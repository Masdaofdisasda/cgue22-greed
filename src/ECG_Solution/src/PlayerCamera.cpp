#include "Camera.h"

void camera_positioner_player::set_position(const glm::vec3 pos) {
	camera_position_ = pos;
}

void camera_positioner_player::update(double delta_seconds, const glm::vec2& mouse_pos, bool mouse_pressed) {

	const glm::vec2 delta = mouse_pos - last_mouse_pos_;
	last_mouse_pos_ = mouse_pos;

	const auto delta_quat = glm::quat(mouse_speed_ * glm::vec3(delta.y, delta.x, 0.0f));
	const glm::quat unclamped_rotation = delta_quat * camera_orientation_;
	const float pitch = glm::pitch(unclamped_rotation);
	const float yaw = glm::yaw(unclamped_rotation);

	if ((std::abs(yaw) >= 0.01 || (std::abs(pitch) <= glm::half_pi<float>()))) // clamp y-rotation
		camera_orientation_ = unclamped_rotation;

	camera_orientation_ = glm::normalize(camera_orientation_);

	const glm::mat4 view = get_view_matrix();
	const glm::vec3 dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
	camera_orientation_ = glm_look_at(camera_position_, camera_position_ + dir, up_);
}

