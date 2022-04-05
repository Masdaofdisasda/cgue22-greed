#include "Physics.h"

static const double PI = 3.141592653589793238463;

Physics::Physics() {
	btDbvtBroadphase* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collision_configuration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collision_configuration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	this->dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);
	this->dynamics_world->setGravity(btVector3(0, -10, 0));

	this->bulletDebugDrawer = new BulletDebugDrawer();
	this->bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	this->dynamics_world->setDebugDrawer(bulletDebugDrawer);
}

void Physics::addDynamicObject(Hierarchy* modelGraphics, btVector3 pos, btCollisionShape* col) {
	addDynamicObject(modelGraphics, pos, col, *new btQuaternion());
}

void Physics::addDynamicObject(Hierarchy* modelGraphics, btVector3 pos, btCollisionShape* col, btQuaternion rot) {
	addPhysicsObject(pos, col, rot, modelGraphics, 1);
}

void Physics::addStaticObject(Hierarchy* modelGraphics, btVector3 pos, btCollisionShape* col) {
	addStaticObject(modelGraphics, pos, col, *new btQuaternion()); 
}

void Physics::addStaticObject(Hierarchy* modelGraphics, btVector3 pos, btCollisionShape* col, btQuaternion rot) {
	addPhysicsObject(pos, col, rot, modelGraphics, 0);
}

void Physics::simulateOneStep(float secondsBetweenFrames) {
	dynamics_world->stepSimulation(secondsBetweenFrames);

	// update positions of all dynamic objects
	for (int i = 0; i < physicsObjects.size(); i++)
		updateModelTransform(&physicsObjects[i]);
}

void Physics::debugDraw() {
	dynamics_world->debugDrawWorld();
	bulletDebugDrawer->draw();
}

void Physics::updateModelTransform(PhysicsObject* physicsObject) {
	// only update objects with graphical representation
	if (physicsObject->graphics == nullptr) 
		return;

	//TODO: rewrite with getOpenGLMatrix() from btTransform
	btRigidBody rb = *physicsObject->rigidbody;
	glm::vec3 pos = Physics::btToGlmVector(rb.getCenterOfMassTransform().getOrigin());
	float deg = (float)(rb.getOrientation().getAngle() * 180 / Physics::PI);
	glm::vec3 axis = Physics::btToGlmVector(rb.getOrientation().getAxis());
	glm::vec3 scale = glm::vec3(0.5);

	glm::mat4 T = glm::translate(glm::mat4(1), pos);
	glm::mat4 R = glm::rotate(glm::mat4(1), glm::radians(deg), axis);
	glm::mat4 S = glm::scale(glm::mat4(1), scale);
	physicsObject->graphics->localTransform = T * R * S;
}

void Physics::addPhysicsObject(btVector3 pos, btCollisionShape* col, btQuaternion rot, Hierarchy* modelGraphics, btScalar mass) {
	// Create rigidbody
	btRigidBody* rigidbody = Physics::makeRigidbody(pos, col, rot, mass);

	// add it to physics world
	dynamics_world->addRigidBody(rigidbody);

	// save rigidbody and graphical model representation in one struct
	PhysicsObject physicsObject;
	physicsObject.graphics = modelGraphics;
	physicsObject.rigidbody = rigidbody;
	physicsObjects.push_back(physicsObject);
}

// static functions

btConvexHullShape* Physics::getHullShapeFromMesh(Mesh* mesh) {
	btConvexHullShape* shape = new btConvexHullShape();
	btScalar* coordinates = (*mesh).getVerticeCoordinates();
	int verticeAmount = (*mesh).getVerticeAmount();
	for (int i = 0; i < verticeAmount; i++)
	{
		btScalar x = coordinates[i * 3];
		btScalar y = coordinates[i * 3 + 1];
		btScalar z = coordinates[i * 3 + 2];
		shape->addPoint(btVector3(x, y, z));
	}

	return shape;
}

btRigidBody* Physics::makeRigidbody(btVector3 pos, btCollisionShape* col, btQuaternion rot, btScalar mass) {
	btTransform* startTransform = new btTransform(rot, pos);
	btMotionState* motionSate = new btDefaultMotionState(*startTransform);
	btVector3 inertia;
	col->calculateLocalInertia(mass, inertia);
	return new btRigidBody(mass, motionSate, col, inertia);
}

glm::vec3 Physics::btToGlmVector(btVector3 input) {
	return glm::vec3((float)input.getX(), (float)input.getY(), (float)input.getZ());
}