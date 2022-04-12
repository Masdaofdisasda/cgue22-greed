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
	glm::vec3 movementDirection = movementStateToDirection(movement);

	if (glm::length(movementDirection) > 0)
		playerObject->rigidbody->applyCentralImpulse(playerSpeed * physics.glmToBt(movementDirection));
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
