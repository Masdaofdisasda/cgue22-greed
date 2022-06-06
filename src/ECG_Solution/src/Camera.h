#pragma once
#include "Utils.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

/* Camera Interface
* every camera has to give a view matrix and position vector for rendering and shading
*/
class camera_positioner_interface
{
public:
	virtual ~camera_positioner_interface() = default;
	virtual glm::mat4 get_view_matrix() const = 0;
	virtual glm::vec3 get_position() const = 0;
	virtual glm::quat get_orientation() const = 0;
	virtual void update(double delta_seconds, const glm::vec2& mouse_pos, bool mouse_pressed) = 0;
};

class camera final
{
public:
	explicit camera(camera_positioner_interface& positioner)
		: positioner_(&positioner)
	{}

	camera(const camera&) = default;
	camera& operator = (const camera&) = default;

	glm::mat4 get_view_matrix() const { return positioner_->get_view_matrix(); }
	glm::vec3 get_position() const { return positioner_->get_position(); }
	glm::quat get_orientation() const { return positioner_->get_orientation(); }
	void set_positioner(camera_positioner_interface* new_positioner) {
		positioner_ = new_positioner;
	}


private:
	camera_positioner_interface* positioner_;
};

/* Camera with first person view
* view position can be moved in all 6 directions
* the player can only add accelertion to the camera, if the player stops pressing a key, the movement slowly stops
*/
class camera_positioner_first_person final : public camera_positioner_interface
{
	public:
		struct movement
		{
			bool forward = false;
			bool backward = false;
			bool left = false;
			bool right = false;
			bool up = false;
			bool down = false;

			bool fast_speed = false;
		} movement;

		float mouse_speed = 4.0f;
		float acceleration = 150.0f;
		float damping = 0.2f; // changes deceleration speed
		float max_speed = 2.0f; // clamps movement
		float fast_coef = 5.0f; // l-shift mode uses this
		
		camera_positioner_first_person(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
			: camera_position_(pos)
			, camera_orientation_(glm_look_at(pos, target, up))
			, up_(up)
		{}
		void set_movement_state(keyboard_input_state input);
		void update(double delta_seconds, const glm::vec2& mouse_pos, bool mouse_pressed) override;
		virtual glm::mat4 get_view_matrix() const override
		{
			const glm::mat4 t = glm::translate(glm::mat4(1.0f), -camera_position_);
			const glm::mat4 r = glm::mat4_cast(camera_orientation_);
			return r * t;
		};
		virtual glm::vec3 get_position() const override
		{
			return camera_position_;
		};
		void set_position(const glm::vec3& pos);
		void reset_mouse_position(const glm::vec2& p) { mouse_pos_ = p; };
		void set_up_vector(const glm::vec3& up);
		inline void flook_at(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);
		glm::quat get_orientation() const override {
			return camera_orientation_;
		}

	private:
		glm::vec2 mouse_pos_ = glm::vec2(0);
		glm::vec3 camera_position_ = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat camera_orientation_ = glm::quat(glm::vec3(0));
		glm::vec3 move_speed_ = glm::vec3(0.0f);
		glm::vec3 up_ = glm::vec3(0.0f, 0.0f, 1.0f);

};

class camera_positioner_player final : public camera_positioner_interface {
public:
	virtual glm::mat4 get_view_matrix() const override
	{
		const glm::mat4 t = glm::translate(glm::mat4(1.0f), -camera_position_);
		const glm::mat4 r = glm::mat4_cast(camera_orientation_);
		return r * t;
	};
	virtual glm::vec3 get_position() const override
	{
		return camera_position_;
	};
	void set_position(glm::vec3 pos);
	void update(double delta_seconds, const glm::vec2& mouse_pos, bool mouse_pressed) override;
	glm::mat4 look_at(glm::vec3 pos, glm::vec3 target, glm::vec3 up);
	glm::quat get_orientation() const override {
		return camera_orientation_;
	}
private:
	glm::vec3 camera_position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat camera_orientation_ = glm::quat(glm::vec3(0.0f, 0.7853982f,0.0f));
	glm::vec2 last_mouse_pos_ = glm::vec2(0);
	float mouse_speed_ = 4.0f;

	glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
};


class camera_positioner_move_to final : public camera_positioner_interface
{
public:
	camera_positioner_move_to(const glm::vec3& pos, const glm::vec3& angles);

	void update(double delta_seconds, const glm::vec2& mouse_pos, bool mouse_pressed) override;

	void set_position(const glm::vec3& p);
	void set_angles(float pitch, float pan, float roll);
	void set_angles(const glm::vec3& angles);
	void set_desired_position(const glm::vec3& p);
	void set_desired_angles(float pitch, float pan, float roll);
	void set_desired_angles(const glm::vec3& angles);

	virtual glm::vec3 get_position() const override { return position_current_; }
	virtual glm::mat4 get_view_matrix() const override { return current_transform_; }

	virtual glm::quat get_orientation() const override;

public:
	float speed = 0.01f;
	float damping_linear = 10.0f;
	glm::vec3 damping_euler_angles = glm::vec3(5.0f, 5.0f, 5.0f);

private:
	glm::vec3 position_current_ = glm::vec3(0.0f);
	glm::vec3 position_desired_ = glm::vec3(0.0f);

	/// pitch, pan, roll
	glm::vec3 angles_current_ = glm::vec3(0.0f);
	glm::vec3 angles_desired_ = glm::vec3(0.0f);

	glm::mat4 current_transform_ = glm::mat4(1.0f);

	static float clip_angle(float d);

	static glm::vec3 clip_angles(const glm::vec3& angles);

	static glm::vec3 angle_delta(const glm::vec3& anglesCurrent, const glm::vec3& anglesDesired);
};