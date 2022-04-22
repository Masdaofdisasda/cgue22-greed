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

Physics::PhysicsObject& Physics::createPhysicsObject(
	Hierarchy* modelGraphics,
	Transformation modelMatrix,
	std::vector<float> colliderVerticePositions,
	ObjectMode mode)
{
	float mass = getMassFromObjectMode(mode);
	btCollisionShape* collider = getCollisionShapeFromMesh(colliderVerticePositions);
	btRigidBody* rigidbody = makeRigidbody(modelMatrix, collider, mass);
	if (mode == Physics::ObjectMode::Dynamic_NoRotation)
		rigidbody->setAngularFactor(0);
	return addPhysicsObject(rigidbody, modelGraphics, mode);
}

Physics::PhysicsObject& Physics::createPhysicsObject(btVector3 pos, btCollisionShape* col, btQuaternion rot, ObjectMode mode) {
	float mass = getMassFromObjectMode(mode);
	btRigidBody* rigidbody = makeRigidbody(pos, col, rot, mass);
	if (mode == Physics::ObjectMode::Dynamic_NoRotation)
		rigidbody->setAngularFactor(0);
	return addPhysicsObject(rigidbody, nullptr, mode);
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

Physics::PhysicsObject* Physics::rayCast(btVector3 start, btVector3 end) {
	btCollisionWorld::ClosestRayResultCallback result(start, end);
	dynamics_world->rayTest(start, end, result);
	
	if (!result.hasHit())
		return nullptr;

	// find object in datastructure, that was hit
	const btCollisionObject* hitObject = result.m_collisionObject;
	//TODO
	return nullptr;
}

void Physics::updateModelTransform(PhysicsObject* physicsObject) {
	// only update objects with graphical representation
	if (physicsObject->modelGraphics == nullptr)
		return;

	//TODO: rewrite with getOpenGLMatrix() from btTransform
	btRigidBody rb = *physicsObject->rigidbody;
	glm::vec3 pos = btToGlm(rb.getCenterOfMassTransform().getOrigin());
	float deg = (float)(rb.getOrientation().getAngle() * 180 / Physics::PI);
	glm::vec3 axis = btToGlm(rb.getOrientation().getAxis());
	glm::vec3 scale = glm::vec3(0.5);

	glm::quat R = glm::quat_cast(glm::rotate(glm::radians(deg), axis));

	physicsObject->modelGraphics->setNodeTRS(pos, R, scale);
	int i;
}

Physics::PhysicsObject& Physics::addPhysicsObject(btRigidBody* rigidbody, Hierarchy* modelGraphics, Physics::ObjectMode mode) {
	// add it to physics world
	dynamics_world->addRigidBody(rigidbody);

	// save rigidbody and graphical model representation in one struct
	PhysicsObject physicsObject;
	physicsObject.modelGraphics = modelGraphics;
	physicsObject.rigidbody = rigidbody;
	physicsObject.mode = mode;
	physicsObjects.push_back(physicsObject);

	return physicsObjects.back();
}

glm::vec3 Physics::getObjectPosition(PhysicsObject* object) {
	return btToGlm(object->rigidbody->getCenterOfMassTransform().getOrigin());
}

float Physics::getMassFromObjectMode(Physics::ObjectMode mode) {
	if (mode == Physics::ObjectMode::Static)
		return 0;
	return 1;
}

/* --------------------------------------------- */
// Static functions
/* --------------------------------------------- */
btConvexHullShape* Physics::getCollisionShapeFromMesh(std::vector<float> verticePositionArray) {
	btConvexHullShape* shape = new btConvexHullShape();
	int verticeAmount = verticePositionArray.size()/3;
	for (int i = 0; i < verticeAmount; i++)
	{
		btScalar x = verticePositionArray[i * 3];
		btScalar y = verticePositionArray[i * 3 + 1];
		btScalar z = verticePositionArray[i * 3 + 2];
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

btRigidBody* Physics::makeRigidbody(Transformation transform, btCollisionShape* col, btScalar mass) {
	glm::vec3 translation = transform.Translate;
	glm::quat rotation = transform.Rotation;
	btTransform* startTransform = new btTransform(glmToBt(rotation), glmToBt(translation));
	btMotionState* motionSate = new btDefaultMotionState(*startTransform);
	btVector3 inertia;
	col->calculateLocalInertia(mass, inertia);
	return new btRigidBody(mass, motionSate, col, inertia);
}

btQuaternion* Physics::emptyQuaternion() {
	return new btQuaternion(btVector3(0, 1, 0), btScalar(0));
}

/* --------------------------------------------- */
// Conversions
/* --------------------------------------------- */

glm::vec3 Physics::btToGlm(btVector3 input) {
	return glm::vec3((float)input.getX(), (float)input.getY(), (float)input.getZ());
}

btQuaternion Physics::glmToBt(glm::quat input) {
	return btQuaternion(input.x, input.y, input.z, input.w);
}

btVector3 Physics::glmToBt(glm::vec3 input) {
	return btVector3(input.x, input.y, input.z);
}