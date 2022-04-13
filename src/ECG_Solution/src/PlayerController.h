#pragma once
#include "Camera.h"
#include "Physics.h"
#include <glm/ext.hpp>

/// <summary>
/// Creates a rigidbody, that can be moved around. Positions the camera as if it was a part of the rigidbody.
/// The camera can rotate freely, it is only translated by this object.
/// </summary>
class PlayerController
{
public:
	PlayerController(Physics& physics, CameraPositioner_Player& camera, glm::vec3 startPosition);
	
	/// <summary>
	/// Tries to move the rigidbody in the desired direction. 
	/// Forwards and backwards depend on the orientation of the camera.
	/// </summary>
	void move(KeyboardInputState inputs);

	/// <summary>
	/// Sets the camera position to be on top of the player rigidbody
	/// </summary>
	void updateCameraPosition();

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
	CameraPositioner_Player& camera;
	PhysicsObject* playerObject;

	// settings
	glm::vec3 rigidbodyToCameraOffset = glm::vec3(0,1,0);
	float playerSpeed = 0.05f; // how fast the player accelerates
	float jumpStrength = 0.1f; // how high the player can jump
	float stopSpeed = 1.01f; // how fast the player decelerates when not giving input (x,z axis)
	float maxSpeed = 15.0f; // how fast the player is allowed to run at max (x,z axis)

	Movement* inputToMovementState(KeyboardInputState inputs);
	glm::vec3 movementStateToDirection(Movement* movement);
	void decelerateXZ();
	void enforceSpeedLimit();
};

