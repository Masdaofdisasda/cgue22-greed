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
	hierarchy* modelGraphics,
	transformation modelMatrix,
	std::vector<float> colliderVerticePositions,
	ObjectMode mode)
{
	float mass = getMassFromObjectMode(mode);
	btVector3 scale = glmToBt(scaleFromTransform(modelMatrix.get_matrix()));
	btCollisionShape* collider = getCollisionShapeFromMesh(colliderVerticePositions, scale);
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
	// look which items are allowed to take part
	for (int i = 0; i < physicsObjects.size(); i++)
		excludeAndIncludePhysicsObject(physicsObjects[i]);

	// simulate
	dynamics_world->stepSimulation(secondsBetweenFrames);

	// update positions of all dynamic objects for rendering
	for (int i = 0; i < physicsObjects.size(); i++)
		updateModelTransform(&physicsObjects[i]);
}

void Physics::excludeAndIncludePhysicsObject(Physics::PhysicsObject &obj) {
	if (obj.modelGraphics == nullptr)
		return;
	//if(physicsObjects[i].mode == Physics::ObjectMode::Dynamic)

	if (!obj.modelGraphics->game_properties.is_active) {
		obj.rigidbody->setActivationState(ISLAND_SLEEPING);
		obj.rigidbody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
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
	return getPhysicsObjectByCollisionObject(hitObject);
}

Physics::PhysicsObject* Physics::getPhysicsObjectByCollisionObject(const btCollisionObject* collider) {
	for (int i = 0; i < physicsObjects.size(); i++)
		if (physicsObjects[i].rigidbody->getCollisionShape() == collider->getCollisionShape())
			return &physicsObjects[i];
	return nullptr;
}

void Physics::updateModelTransform(PhysicsObject* physicsObject) {
	// only update objects with graphical representation
	if (physicsObject->modelGraphics == nullptr)
		return;

	btRigidBody rb = *physicsObject->rigidbody;
	glm::vec3 pos = btToGlm(rb.getCenterOfMassTransform().getOrigin());
	//float deg = (float)(rb.getOrientation().getAngle() * 180 / Physics::PI);
	float deg = (float)rb.getOrientation().getAngle();
	glm::vec3 axis = btToGlm(rb.getOrientation().getAxis());
	glm::vec3 scale = glm::vec3(1.0);

	glm::quat rot = glm::angleAxis(deg, axis);

	physicsObject->modelGraphics->set_node_trs(pos, rot, scale);
}

Physics::PhysicsObject& Physics::addPhysicsObject(btRigidBody* rigidbody, hierarchy* modelGraphics, Physics::ObjectMode mode) {
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
btConvexHullShape* Physics::getCollisionShapeFromMesh(std::vector<float> verticePositionArray, btVector3 scale) {
	btConvexHullShape* shape = new btConvexHullShape();
	glm::mat4 scalingMatrix = glm::scale(glm::mat4(), btToGlm(scale));

	int verticeAmount = verticePositionArray.size() / 3;
	for (int i = 0; i < verticeAmount; i++)
	{
		float x = verticePositionArray[i * 3];
		float y = verticePositionArray[i * 3 + 1];
		float z = verticePositionArray[i * 3 + 2];
		glm::vec4 scaledPoint = scalingMatrix * glm::vec4(x, y, z, 1);
		shape->addPoint(btVector3(scaledPoint.x, scaledPoint.y, scaledPoint.z));
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

btRigidBody* Physics::makeRigidbody(transformation transform, btCollisionShape* col, btScalar mass) {
	glm::vec3 translation = transform.translate;
	glm::quat rotation = transform.rotation;
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