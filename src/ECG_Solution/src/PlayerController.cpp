#include "PlayerController.h"

player_controller::player_controller(Physics& physics, camera_positioner_player& camera, glm::vec3 start_position)
	: physics_(physics), camera_positioner_(camera)
{
	btCollisionShape* collisionShape = new btCapsuleShape(0.5, 1);
	player_object_ = &physics.createPhysicsObject(
		physics.glmToBt(start_position),
		collisionShape,
		*Physics::emptyQuaternion(),
		Physics::ObjectMode::Dynamic_NoRotation
	);
}

void player_controller::move(const keyboard_input_state inputs, const float delta_time)
{
	// hinder rigidbody from sleeping
	player_object_->rigidbody->activate(true);

	// translate movement input
	movement movement;
	input_to_movement_state(inputs, movement);

	// running
	const glm::vec3 movement_direction = movement_state_to_direction(&movement);
	const bool shouldAccelerate = glm::length(movement_direction) > 0;
	if (shouldAccelerate)
		accelerate(movement_direction, delta_time);
	else
		decelerate_xz(delta_time);
	enforce_speed_limit();

	// jumping
	is_grounded_ = is_ground_under_player();
	jump_cooldown_time_ = jump_cooldown_time_ > 0 ? jump_cooldown_time_ - delta_time : 0;
	const bool allowedToJump = is_grounded_ && jump_cooldown_time_ <= 0;
	if (movement.jump && allowedToJump) {
		jump();
		jump_cooldown_time_ = jump_max_cooldown_time_;
	}
}

void player_controller::accelerate(glm::vec3 movement_direction, const float delta_time) {
	player_object_->rigidbody->applyCentralImpulse(player_speed_ * static_cast<double>(delta_time) * physics_.glmToBt(movement_direction));
}

void player_controller::jump() {
	printf("jump");
	player_object_->rigidbody->applyCentralImpulse(btVector3(0, jump_strength_, 0));
}

bool player_controller::is_ground_under_player() {
	const btVector3 ray_cast_start_point = player_object_->rigidbody->getCenterOfMassTransform().getOrigin();
	const btVector3 ray_cast_end_point = ray_cast_start_point + btVector3(0, -max_ground_distance_, 0);
	Physics::PhysicsObject* hit_object = physics_.rayCast(ray_cast_start_point, ray_cast_end_point);

	if (hit_object == nullptr || hit_object->modelGraphics == nullptr)
		return false;
	if (!hit_object->modelGraphics->game_properties.is_active)
		return false;
	if (hit_object->modelGraphics->game_properties.is_ground)
		return true;
	return false;
}

void player_controller::update_camera_positioner()
{
	const glm::vec3 rb_position = physics_.getObjectPosition(player_object_);
	camera_positioner_.set_position(rb_position + rigidbody_to_camera_offset_);
}

bool player_controller::has_collectable_item_in_reach() const
{
	return get_collectable_in_front_of_player() != nullptr;
}

void player_controller::try_collect_item(const mouse_state mouse_state, const keyboard_input_state keyboard_state, item_collection& item_collection)
{
	const bool want_to_collect = mouse_state.pressed_left || keyboard_state.pressing_e;
	if (!want_to_collect)
		return;

	Physics::PhysicsObject* item = get_collectable_in_front_of_player();

	// item there?
	if (item == nullptr)
		return;

	// collect
	printf("collected item\n");
	item_collection.collect(item);
	item_weight_ = item_collection.get_total_weight();
}

Physics::PhysicsObject* player_controller::get_collectable_in_front_of_player() const
{
	const glm::vec3 camera_position = camera_positioner_.get_position();
	const glm::mat4 v = glm::mat4_cast(camera_positioner_.get_orientation());
	const glm::vec3 camera_aim_direction = -glm::vec3(v[0][2], v[1][2], v[2][2]);

	const btVector3 ray_cast_start_point = physics_.glmToBt(camera_position);
	const btVector3 ray_cast_end_point = physics_.glmToBt(camera_position + camera_aim_direction * reach_);
	Physics::PhysicsObject* hit_object = physics_.rayCast(ray_cast_start_point, ray_cast_end_point);

	if (hit_object == nullptr || hit_object->modelGraphics == nullptr)
		return nullptr;
	if (!hit_object->modelGraphics->game_properties.is_active)
		return nullptr;
	if (hit_object->modelGraphics->game_properties.is_collectable)
		return hit_object;
	return nullptr;
}

void player_controller::input_to_movement_state(const keyboard_input_state inputs, movement& movement)
{
	movement.forwards = inputs.pressing_w;
	movement.backwards = inputs.pressing_s;
	movement.left = inputs.pressing_a;
	movement.right = inputs.pressing_d;
	movement.jump = inputs.pressing_space;

}

glm::vec3 player_controller::movement_state_to_direction(const movement* movement) const
{
	const auto v = glm::mat4_cast(camera_positioner_.get_orientation());
	const auto forward = -glm::vec3(v[0][2], 0, v[2][2]);
	const auto right = glm::vec3(v[0][0], 0, v[2][0]);

	auto result = glm::vec3(0, 0, 0);
	if (movement->forwards)
		result += forward;
	if (movement->backwards)
		result += -forward;
	if (movement->right)
		result += right;
	if (movement->left)
		result += -right;

	if (glm::length(result) > 0)
		return glm::normalize(result);
	return result;
}

void player_controller::decelerate_xz(float delta_time) const
{
	const btVector3 velocity = player_object_->rigidbody->getLinearVelocity();
	const auto xz_velocity = glm::vec2(static_cast<float>(velocity.getX()), static_cast<float>(velocity.getZ()));

	const bool player_is_standing = glm::length(xz_velocity) <= 0;
	if (player_is_standing)
		return;

	const float new_xz_magnitude = glm::length(xz_velocity) / (1 + stop_speed_ * delta_time);
	const glm::vec2 new_xz_velocity = glm::normalize(xz_velocity) * new_xz_magnitude;
	player_object_->rigidbody->setLinearVelocity(btVector3(
		new_xz_velocity.x,
		velocity.getY(),
		new_xz_velocity.y
	));
}

void player_controller::enforce_speed_limit() const
{
	const btVector3 velocity = player_object_->rigidbody->getLinearVelocity();
	const auto xz_velocity = glm::vec2(static_cast<float>(velocity.getX()), static_cast<float>(velocity.getZ()));
	const float current_speed = glm::length(xz_velocity);
	const float new_max_speed = max_speed_ - item_weight_;

	const bool player_is_standing = current_speed <= 0;
	const bool speed_is_too_high = current_speed > new_max_speed;
	if (player_is_standing || !speed_is_too_high)
		return;

	const glm::vec2 new_xz_velocity = glm::normalize(xz_velocity) * new_max_speed;
	player_object_->rigidbody->setLinearVelocity(btVector3(
		new_xz_velocity.x,
		velocity.getY(),
		new_xz_velocity.y
	));
}
