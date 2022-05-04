#pragma once
#include "Camera.h"
#include "Physics.h"
#include <glm/ext.hpp>
#include "ItemCollection.h"

/// <summary>
/// Creates a rigidbody, that can be moved around. Positions the camera as if it was a part of the rigidbody.
/// The camera can rotate freely, it is only translated by this object.
/// </summary>
class player_controller
{
public:
	player_controller(Physics& physics, camera_positioner_player& camera, glm::vec3 start_position);
	
	/// <summary>
	/// Tries to move the rigidbody in the desired direction. 
	/// Forwards and backwards depend on the orientation of the camera.
	/// </summary>
	void move(keyboard_input_state inputs, float delta_time);

	/// <summary>
	/// Sets the camera position to be on top of the player rigidbody
	/// </summary>
	void update_camera_positioner();

	bool has_collectable_item_in_reach() const;

	void try_collect_item(mouse_state mouse_state, keyboard_input_state keyboard_state, item_collection& item_collection);

private:
	struct movement
	{
		bool forwards = false;
		bool backwards = false;
		bool left = false;
		bool right = false;
		bool jump = false;
	};

	Physics& physics_;
	camera_positioner_player& camera_positioner_;
	Physics::PhysicsObject* player_object_;
	bool is_grounded_ = false; // is the player standing on a surface, he can jump off of?
	float jump_cooldown_time_ = 0; // how much time is left until the jump is usable again

	// settings
	glm::vec3 rigidbody_to_camera_offset_ = glm::vec3(0,1,0);
	float player_speed_ = 20.0f; // how fast the player accelerates (x,z axis)
	float jump_strength_ = 8.0f; // how high the player can jump (y axis)
	float stop_speed_ = 5.0f; // how fast the player decelerates when not giving input (x,z axis)
	float max_speed_ = 15.0f; // how fast the player is allowed to run at max (x,z axis)
	float reach_ = 5.0f; // maximum distance that items can be away and still be collected
	float item_weight_ = 0; // how much all the items weigh together (influences movement)
	float jump_max_cooldown_time_ = 1.0f; // how long the jump will be on cooldown after initialization
	float max_ground_distance_ = 1.25f; // how far the ground can be away, so that the player can still jump off of it.

	Physics::PhysicsObject* get_collectable_in_front_of_player() const;
	static void input_to_movement_state(keyboard_input_state inputs, movement& movement);
	glm::vec3 movement_state_to_direction(const movement* movement) const;
	void decelerate_xz(float delta_time) const;
	void enforce_speed_limit() const;
	void accelerate(glm::vec3 movement_direction, const float delta_time);
	void jump();
	bool is_ground_under_player();
};

