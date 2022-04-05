#pragma once
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include "BulletDebugDrawer.h"
#include "Mesh.h"

struct PhysicsObject {
	btRigidBody* rigidbody;
	Hierarchy* graphics;
};

class Physics
{
public:
	static const double PI;

	Physics();

	void debugDraw();

	void addDynamicObject(Hierarchy* modelGraphics, btVector3 pos, btCollisionShape* col);
	void addDynamicObject(Hierarchy* modelGraphics, btVector3 pos, btCollisionShape* col, btQuaternion rot);

	void addStaticObject(Hierarchy* modelGraphics, btVector3 pos, btCollisionShape* col);
	void addStaticObject(Hierarchy* modelGraphics, btVector3 pos, btCollisionShape* col, btQuaternion rot);

	void simulateOneStep(float secondsBetweenFrames);

	static btConvexHullShape* getHullShapeFromMesh(Mesh* mesh);

	static btRigidBody* makeRigidbody(btVector3 pos, btCollisionShape* col, btQuaternion rot, btScalar mass);

	static glm::vec3 btToGlmVector(btVector3 input);

private:
	btDiscreteDynamicsWorld* dynamics_world;
	BulletDebugDrawer* bulletDebugDrawer;
	std::vector <PhysicsObject> physicsObjects;

	void addPhysicsObject(btVector3 pos, btCollisionShape* col, btQuaternion rot, Hierarchy* modelGraphics, btScalar mass);

	void updateModelTransform(PhysicsObject* model);
};

