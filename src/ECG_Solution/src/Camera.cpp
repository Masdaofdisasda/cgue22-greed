#include "Camera.h"

#include "BulletDebugDrawer.h"
#include "glm/gtx/euler_angles.hpp"

void camera_positioner_first_person::set_movement_state(keyboard_input_state input)
{
	movement.forward = input.pressing_w;
	movement.backward = input.pressing_s;
	movement.left = input.pressing_a;
	movement.right = input.pressing_d;
	movement.up = input.pressing_1;
	movement.down = input.pressing_2;
	movement.fast_speed = input.pressing_shift;

	if (input.pressing_space)
		set_up_vector(glm::vec3(0.0f, 1.0f, 0.0f));
}

void camera_positioner_first_person::update(double delta_seconds, const glm::vec2& mouse_pos, bool mouse_pressed) {
	// gets called once per loop

	// rotate camera by creating a quaternion from the mouse deltas
	if (mouse_pressed)
	{
		const glm::vec2 delta = mouse_pos - mouse_pos_;
		glm::quat deltaQuat = glm::quat(glm::vec3(mouse_speed * delta.y, mouse_speed * delta.x, 0.0f));
		glm::quat unclamped_rotation = deltaQuat * camera_orientation_;
		float pitch = glm::pitch(unclamped_rotation);
		float yaw = glm::yaw(unclamped_rotation);
		
		if ((std::abs(yaw) >= 0.01 || (std::abs(pitch) <= glm::half_pi<float>()))) // clamp y-rotation
			camera_orientation_ = unclamped_rotation;
		
		camera_orientation_ = glm::normalize(camera_orientation_);
		set_up_vector(up_);
	}
	mouse_pos_ = mouse_pos;

	// translate camera by adding or substracting to the orthographic vectors
	const glm::mat4 v = glm::mat4_cast(camera_orientation_);

	const glm::vec3 forward = -glm::vec3(v[0][2], v[1][2], v[2][2]);
	const glm::vec3 right = glm::vec3(v[0][0], v[1][0], v[2][0]);
	const glm::vec3 up = glm::cross(right, forward);

	glm::vec3 accel(0.0f);

	if (movement.forward) accel += forward;
	if (movement.backward) accel -= forward;

	if (movement.left) accel -= right;
	if (movement.right) accel += right;

	if (movement.up) accel += up;
	if (movement.down) accel -= up;

	if (movement.fast_speed) accel *= fast_coef;

	if (accel == glm::vec3(0))
	{
		// decelerate naturally according to the damping value
		move_speed_ -= move_speed_ * std::min((1.0f / damping) * static_cast<float>(delta_seconds), 1.0f);
	}
	else
	{
		// acceleration
		move_speed_ += accel * acceleration * static_cast<float>(delta_seconds);
		const float maxSpeed = movement.fast_speed ? max_speed * fast_coef : max_speed;
		if (glm::length(move_speed_) > maxSpeed) move_speed_ = glm::normalize(move_speed_) * maxSpeed;
	}

	camera_position_ += move_speed_ * static_cast<float>(delta_seconds);
}

void camera_positioner_first_person::set_position(const glm::vec3& pos)
{
	camera_position_ = pos;
}

void camera_positioner_first_person::set_up_vector(const glm::vec3& up)
{
	const glm::mat4 view = get_view_matrix();
	const glm::vec3 dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
	camera_orientation_ = glm_look_at(camera_position_, camera_position_ + dir, up);
}

inline void camera_positioner_first_person::flook_at(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up) {
	camera_position_ = pos;
	camera_orientation_ = glm_look_at(pos, target, up);
}

camera_positioner_move_to::camera_positioner_move_to(const glm::vec3& pos, const glm::vec3& angles): positionCurrent_(pos)
	, positionDesired_(pos)
	, anglesCurrent_(angles)
	, anglesDesired_(angles)
{}

void camera_positioner_move_to::update(double delta_seconds, const glm::vec2& mouse_pos, bool mouse_pressed)
{
	positionCurrent_ += dampingLinear_ * delta_seconds * (positionDesired_ - positionCurrent_) * speed_;

	// normalization is required to avoid "spinning" around the object 2pi times
	anglesCurrent_ = clipAngles(anglesCurrent_);
	anglesDesired_ = clipAngles(anglesDesired_);

	// update angles
	anglesCurrent_ -= angleDelta(anglesCurrent_, anglesDesired_) * dampingEulerAngles_ * delta_seconds * speed_;

	// normalize new angles
	anglesCurrent_ = clipAngles(anglesCurrent_);

	const glm::vec3 a = glm::radians(anglesCurrent_);

	currentTransform_ = glm::translate(glm_euler_angle_xyz(a.y, a.x, a.z), -positionCurrent_);
}

void camera_positioner_move_to::setPosition(const glm::vec3& p)
{ positionCurrent_ = p; }

void camera_positioner_move_to::setAngles(float pitch, float pan, float roll)
{ anglesCurrent_ = glm::vec3(pitch, pan, roll); }

void camera_positioner_move_to::setAngles(const glm::vec3& angles)
{ anglesCurrent_ = angles; }

void camera_positioner_move_to::setDesiredPosition(const glm::vec3& p)
{ positionDesired_ = p; }

void camera_positioner_move_to::setDesiredAngles(float pitch, float pan, float roll)
{ anglesDesired_ = glm::vec3(pitch, pan, roll); }

void camera_positioner_move_to::setDesiredAngles(const glm::vec3& angles)
{ anglesDesired_ = angles; }

glm::quat camera_positioner_move_to::get_orientation() const
{
	return glm::quat(glm::vec3(0.0f));
}

float camera_positioner_move_to::clipAngle(float d)
{
	if (d < -180.0f) return d + 360.0f;
	if (d > +180.0f) return d - 360.f;
	return d;
}

glm::vec3 camera_positioner_move_to::clipAngles(const glm::vec3& angles)
{
	return glm::vec3(
		std::fmod(angles.x, 360.0f),
		std::fmod(angles.y, 360.0f),
		std::fmod(angles.z, 360.0f)
	);
}

glm::vec3 camera_positioner_move_to::angleDelta(const glm::vec3& anglesCurrent, const glm::vec3& anglesDesired)
{
	const glm::vec3 d = clipAngles(anglesCurrent) - clipAngles(anglesDesired);
	return glm::vec3(clipAngle(d.x), clipAngle(d.y), clipAngle(d.z));
}

