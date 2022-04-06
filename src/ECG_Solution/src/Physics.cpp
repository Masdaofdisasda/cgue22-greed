#include "Physics.h"

const double Physics::PI = 3.141592653589793238463;

Physics::Physics() {
	btDbvtBroadphase* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collision_configuration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collision_configuration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));

	bulletDebugDrawer = new BulletDebugDrawer();
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	dynamics_world->setDebugDrawer(bulletDebugDrawer);
}

void Physics::createPhysicsObject(Hierarchy* modelGraphics, std::vector<float> colliderVerticePositions, ObjectMode mode) {
	btCollisionShape* collider = getCollisionShapeFromMesh(colliderVerticePositions);
	float mass = mode;
	btRigidBody* rigidbody = makeRigidbody(modelGraphics->localTransform, collider, mass);
	addPhysicsObject(rigidbody, modelGraphics);
}

void Physics::createPhysicsObject(btVector3 pos, btCollisionShape* col, btQuaternion rot, ObjectMode mode) {
	float mass = mode;
	btRigidBody* rigidbody = makeRigidbody(pos, col, rot, mass);
	addPhysicsObject(rigidbody, nullptr);
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
	glm::vec3 pos = btToGlm(rb.getCenterOfMassTransform().getOrigin());
	float deg = (float)(rb.getOrientation().getAngle() * 180 / Physics::PI);
	glm::vec3 axis = btToGlm(rb.getOrientation().getAxis());
	glm::vec3 scale = glm::vec3(0.5);

	glm::mat4 T = glm::translate(glm::mat4(1), pos);
	glm::mat4 R = glm::rotate(glm::mat4(1), glm::radians(deg), axis);
	glm::mat4 S = glm::scale(glm::mat4(1), scale);
	physicsObject->graphics->localTransform = T * R * S;
}

void Physics::addPhysicsObject(btRigidBody* rigidbody, Hierarchy* modelGraphics) {
	// add it to physics world
	dynamics_world->addRigidBody(rigidbody);

	// save rigidbody and graphical model representation in one struct
	PhysicsObject physicsObject;
	physicsObject.graphics = modelGraphics;
	physicsObject.rigidbody = rigidbody;
	physicsObjects.push_back(physicsObject);
}

/* --------------------------------------------- */
// Static functions
/* --------------------------------------------- */
btConvexHullShape* Physics::getCollisionShapeFromMesh(std::vector<float> verticePositionArray) {
	btConvexHullShape* shape = new btConvexHullShape();
	btScalar* coordinates = verticePosArrayToScalarArray(verticePositionArray);
	int verticeAmount = verticePositionArray.size();
	for (int i = 0; i < verticeAmount; i++)
	{
		btScalar x = coordinates[i * 3];
		btScalar y = coordinates[i * 3 + 1];
		btScalar z = coordinates[i * 3 + 2];
		shape->addPoint(btVector3(x, y, z));
	}
	delete[] coordinates;
	return shape;
}

btRigidBody* Physics::makeRigidbody(btVector3 pos, btCollisionShape* col, btQuaternion rot, btScalar mass) {
	btTransform* startTransform = new btTransform(rot, pos);
	btMotionState* motionSate = new btDefaultMotionState(*startTransform);
	btVector3 inertia;
	col->calculateLocalInertia(mass, inertia);
	return new btRigidBody(mass, motionSate, col, inertia);
}

btRigidBody* Physics::makeRigidbody(glm::mat4 transform, btCollisionShape* col, btScalar mass) {
	glm::vec3 translation = translationFromTransform(transform);
	glm::quat rotation = rotationFromTransform(transform);
	btTransform* startTransform = new btTransform(glmToBt(rotation), glmToBt(translation));
	btMotionState* motionSate = new btDefaultMotionState(*startTransform);
	btVector3 inertia;
	col->calculateLocalInertia(mass, inertia);
	return new btRigidBody(mass, motionSate, col, inertia);
}

/* --------------------------------------------- */
// Conversions
/* --------------------------------------------- */
btScalar* Physics::verticePosArrayToScalarArray(std::vector<float> verticePositionArray) {
	int verticeAmount = verticePositionArray.size(); // 3 coords per vertice
	btScalar* scalars = new btScalar[verticeAmount * 3];
	for (int i = 0; i < verticeAmount; i++)
	{
		scalars[i * 3] = btScalar(verticePositionArray[i]);
		scalars[i * 3 + 1] = btScalar(verticePositionArray[i]);
		scalars[i * 3 + 2] = btScalar(verticePositionArray[i]);
	}
	return scalars;
}

glm::vec3 Physics::btToGlm(btVector3 input) {
	return glm::vec3((float)input.getX(), (float)input.getY(), (float)input.getZ());
}

btQuaternion Physics::glmToBt(glm::quat input) {
	return btQuaternion(input.x, input.y, input.z, input.w);
}

btVector3 Physics::glmToBt(glm::vec3 input) {
	return btVector3(input.x, input.y, input.z);
}