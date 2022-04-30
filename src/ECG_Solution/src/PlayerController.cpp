#include "PlayerController.h"

PlayerController::PlayerController(Physics& physics, camera_positioner_player& camera, glm::vec3 startPosition)
	: physics(physics), cameraPositioner(camera)
{
	btCollisionShape* collisionShape = new btCapsuleShape(0.5, 1);
	playerObject = &physics.createPhysicsObject(
		physics.glmToBt(startPosition),
		collisionShape,
		*physics.emptyQuaternion(),
		Physics::ObjectMode::Dynamic_NoRotation
	);
}

void PlayerController::move(KeyboardInputState inputs, float deltatime)
{
	Movement* movement = inputToMovementState(inputs);

	// running
	glm::vec3 movementDirection = movementStateToDirection(movement);
	bool accelerate = glm::length(movementDirection) > 0;
	if (accelerate) 
		playerObject->rigidbody->applyCentralImpulse(playerSpeed * (double)deltatime * physics.glmToBt(movementDirection));
	else
		decelerateXZ(deltatime);
	enforceSpeedLimit();

	// jumping
	if (movement->jump)
		playerObject->rigidbody->applyCentralImpulse(btVector3(0, jumpStrength, 0));
}

void PlayerController::updateCameraPositioner()
{
	glm::vec3 rbPosition = physics.getObjectPosition(playerObject);
	cameraPositioner.set_position(rbPosition + rigidbodyToCameraOffset);
}

bool PlayerController::hasCollectableItemInReach() {
	return geCollectableInFrontOfPlayer() != nullptr;
}

void PlayerController::tryCollectItem(MouseState mouseState, KeyboardInputState keyboardState, ItemCollection& itemCollection)
{
	bool wantToCollect = mouseState.pressedLeft || keyboardState.pressingE;
	if (!wantToCollect)
		return;

	Physics::PhysicsObject* item = geCollectableInFrontOfPlayer();

	// item there?
	if (item == nullptr)
		return;

	// collect
	printf("collected item\n");
	itemCollection.collect(item);
	itemWeight = itemCollection.getTotalWeight();
}

Physics::PhysicsObject* PlayerController::geCollectableInFrontOfPlayer() {
	glm::vec3 cameraPosition = cameraPositioner.get_position();
	const glm::mat4 v = glm::mat4_cast(cameraPositioner.get_orientation());
	const glm::vec3 cameraAimDirection = -glm::vec3(v[0][2], v[1][2], v[2][2]);

	btVector3 rayCastStartPoint = physics.glmToBt(cameraPosition);
	btVector3 rayCastEndPoint = physics.glmToBt(cameraPosition + cameraAimDirection * reach);
	Physics::PhysicsObject* hitObject = physics.rayCast(rayCastStartPoint, rayCastEndPoint);

	if (hitObject == nullptr || hitObject->modelGraphics == nullptr)
		return nullptr;
	if (!hitObject->modelGraphics->game_properties.is_active)
		return nullptr;
	if (hitObject->modelGraphics->game_properties.is_collectable)
		return hitObject;
	return nullptr;
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
	const glm::mat4 v = glm::mat4_cast(cameraPositioner.get_orientation());
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

void PlayerController::decelerateXZ(float deltatime)
{
	btVector3 velocity = playerObject->rigidbody->getLinearVelocity();
	glm::vec2 xzVelocity = glm::vec2((float)velocity.getX(), (float)velocity.getZ());

	bool playerIsStanding = glm::length(xzVelocity) <= 0;
	if (playerIsStanding)
		return;

	float newXZMagnitude = glm::length(xzVelocity) / (1 + stopSpeed * deltatime);
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
	float currentSpeed = glm::length(xzVelocity);
	float newMaxSpeed = maxSpeed - itemWeight;

	bool playerIsStanding = currentSpeed <= 0;
	bool speedIsTooHigh = currentSpeed > newMaxSpeed;
	if (playerIsStanding || !speedIsTooHigh)
		return;

	glm::vec2 newXZVelocity = glm::normalize(xzVelocity) * newMaxSpeed;
	playerObject->rigidbody->setLinearVelocity(btVector3(
		newXZVelocity.x,
		velocity.getY(),
		newXZVelocity.y
	));
}
