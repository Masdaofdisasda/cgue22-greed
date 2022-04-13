#include "PlayerController.h"

PlayerController::PlayerController(Physics& physics, CameraPositioner_Player& camera, glm::vec3 startPosition)
	: physics(physics), camera(camera)
{
	btCollisionShape* collisionShape = new btCapsuleShape(0.5, 1);
	playerObject = &physics.createPhysicsObject(
		physics.glmToBt(startPosition),
		collisionShape,
		*physics.emptyQuaternion(),
		Physics::ObjectMode::Dynamic_NoRotation
	);
}

void PlayerController::move(KeyboardInputState inputs)
{
	Movement* movement = inputToMovementState(inputs);

	// running
	glm::vec3 movementDirection = movementStateToDirection(movement);
	bool accelerate = glm::length(movementDirection) > 0;
	if (accelerate)
		playerObject->rigidbody->applyCentralImpulse(playerSpeed * physics.glmToBt(movementDirection));
	else
		decelerateXZ();
	enforceSpeedLimit();

	// jumping
	if (movement->jump)
		playerObject->rigidbody->applyCentralImpulse(jumpStrength * btVector3(0, 1, 0));
}

void PlayerController::updateCameraPosition()
{
	glm::vec3 rbPosition = physics.getObjectPosition(playerObject);
	camera.setPosition(rbPosition + rigidbodyToCameraOffset);
}

PlayerController::Movement* PlayerController::inputToMovementState(KeyboardInputState inputs)
{
	Movement* movement = new Movement;
	movement->forwards = inputs.pressingW;
	movement->backwards = inputs.pressingS;
	movement->left = inputs.pressingA;
	movement->right = inputs.pressingD;
	movement->jump = inputs.pressingSpace;

	return movement;
}

glm::vec3 PlayerController::movementStateToDirection(Movement* movement)
{
	const glm::mat4 v = glm::mat4_cast(camera.getOrientation());
	const glm::vec3 forward = -glm::vec3(v[0][2], 0, v[2][2]);
	const glm::vec3 right = glm::vec3(v[0][0], 0, v[2][0]);

	glm::vec3 result = glm::vec3(0, 0, 0);
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

void PlayerController::decelerateXZ()
{
	btVector3 velocity = playerObject->rigidbody->getLinearVelocity();
	glm::vec2 xzVelocity = glm::vec2((float)velocity.getX(), (float)velocity.getZ());

	bool playerIsStanding = glm::length(xzVelocity) <= 0;
	if (playerIsStanding)
		return;

	float newXZMagnitude = glm::length(xzVelocity) / stopSpeed;
	glm::vec2 newXZVelocity = glm::normalize(xzVelocity) * newXZMagnitude;
	playerObject->rigidbody->setLinearVelocity(btVector3(
		newXZVelocity.x,
		velocity.getY(),
		newXZVelocity.y
	));
}

void PlayerController::enforceSpeedLimit()
{
	btVector3 velocity = playerObject->rigidbody->getLinearVelocity();
	glm::vec2 xzVelocity = glm::vec2((float)velocity.getX(), (float)velocity.getZ());

	bool playerIsStanding = glm::length(xzVelocity) <= 0;
	bool speedIsTooHigh = glm::length(xzVelocity) > maxSpeed;
	if (playerIsStanding || !speedIsTooHigh)
		return;

	glm::vec2 newXZVelocity = glm::normalize(xzVelocity) * maxSpeed;
	playerObject->rigidbody->setLinearVelocity(btVector3(
		newXZVelocity.x,
		velocity.getY(),
		newXZVelocity.y
	));
}
