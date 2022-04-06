#pragma once
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include "BulletDebugDrawer.h"
#include "Mesh.h"

/// <summary>
/// A struct linking the physics and the graphics representation of an object in the game
/// </summary>
struct PhysicsObject {
	btRigidBody* rigidbody;
	Hierarchy* graphics;
};

/// <summary>
/// An abstraction of the currently used physics engine
/// </summary>
class Physics
{
public:
	static const double PI;

	/// <summary>
	/// Static = never moves, not influenced by gravity,
	/// dynamic objects collide with it, does not collide with other static objects
	/// Dynamic = moves, is influenced by gravity, collides with everything
	/// </summary>
	enum ObjectMode { Static = 0, Dynamic = 1};

	Physics();

	/// <summary>
	/// Draws a wireframe representation of all colliders
	/// </summary>
	void debugDraw();

	/// <summary>
	/// Makes a physics object, that has the position and orientation of the input model.
	/// The collision shape will be generated from the collider vertice positions
	/// The object mode determines if the object will move at all
	/// </summary>
	void createPhysicsObject(Hierarchy* modelGraphics, std::vector<float> colliderVerticePositions, ObjectMode mode);
	void createPhysicsObject(btVector3 pos, btCollisionShape* col, btQuaternion rot, ObjectMode mode);

	/// <summary>
	/// Simulates one timestep in the physics engine and updates the transformation of all physics objects.
	/// </summary>
	void simulateOneStep(float secondsBetweenFrames);

private:
	btDiscreteDynamicsWorld* dynamics_world;
	BulletDebugDrawer* bulletDebugDrawer;
	std::vector <PhysicsObject> physicsObjects;

	/// <summary>
	/// Creates and returns a bullet rigidbody
	/// </summary>
	btRigidBody* makeRigidbody(btVector3 pos, btCollisionShape* col, btQuaternion rot, btScalar mass);
	btRigidBody* makeRigidbody(glm::mat4 transform, btCollisionShape* col, btScalar mass);

	/// <summary>
	/// Returns a collision shape generated from the input mesh
	/// </summary>
	btConvexHullShape* getCollisionShapeFromMesh(std::vector<float> verticePositionArray);

	btScalar* verticePosArrayToScalarArray(std::vector<float> verticePositionArray);

	/// <summary>
	/// Adds a rigidbody (created from the input parameters) to the physics world.
	/// Also adds the rigidbody and the modelGraphics to a list to keep track of them.
	/// </summary>
	void addPhysicsObject(btRigidBody* rigidbody, Hierarchy* modelGraphics);

	/// <summary>
	/// Sets the transformation matrix of the visual representation
	/// to the current matrix of the physics representation
	/// </summary>
	void updateModelTransform(PhysicsObject* model);

	glm::vec3 btToGlm(btVector3 input);
	btVector3 glmToBt(glm::vec3 input);
	btQuaternion glmToBt(glm::quat input);
};