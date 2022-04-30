#pragma once
#include "Camera.h"
#include "Physics.h"
#include <glm/ext.hpp>
#include "ItemCollection.h"

/// <summary>
/// Creates a rigidbody, that can be moved around. Positions the camera as if it was a part of the rigidbody.
/// The camera can rotate freely, it is only translated by this object.
/// </summary>
class PlayerController
{
public:
	PlayerController(Physics& physics, camera_positioner_player& camera, glm::vec3 startPosition);
	
	/// <summary>
	/// Tries to move the rigidbody in the desired direction. 
	/// Forwards and backwards depend on the orientation of the camera.
	/// </summary>
	void move(KeyboardInputState inputs, float deltaTime);

	/// <summary>
	/// Sets the camera position to be on top of the player rigidbody
	/// </summary>
	void updateCameraPositioner();

	bool hasCollectableItemInReach();

	void tryCollectItem(MouseState mouseState, KeyboardInputState keyboardState, ItemCollection& itemCollection);

private:
	struct Movement
	{
		bool forwards = false;
		bool backwards = false;
		bool left = false;
		bool right = false;
		bool jump = false;
	};

	Physics& physics;
	camera_positioner_player& cameraPositioner;
	Physics::PhysicsObject* playerObject;

	// settings
	glm::vec3 rigidbodyToCameraOffset = glm::vec3(0,1,0);
	float playerSpeed = 20.0f; // how fast the player accelerates (x,z axis)
	float jumpStrength = 0.1f; // how high the player can jump (y axis)
	float stopSpeed = 5.0f; // how fast the player decelerates when not giving input (x,z axis)
	float maxSpeed = 15.0f; // how fast the player is allowed to run at max (x,z axis)
	float reach = 5.0f; // maximum distance that items can be away and still be collected
	float itemWeight = 0; // how much all the items weigh together (influences movement)

	Physics::PhysicsObject* geCollectableInFrontOfPlayer();
	Movement* inputToMovementState(KeyboardInputState inputs);
	glm::vec3 movementStateToDirection(Movement* movement);
	void decelerateXZ(float deltatime);
	void enforceSpeedLimit();
};

