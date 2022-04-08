#pragma once
#include <bullet/LinearMath/btIDebugDraw.h>
#include <vector>
#include "Utils.h"
#include "Program.h"

class BulletDebugDrawer : public btIDebugDraw
{
private:
	int m_debugMode = 0;
	std::vector <glm::vec3> vertices;
	Program program;
	GLuint vbo_ID = 0;
	GLuint vao_ID = 0;
public:
	BulletDebugDrawer();
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	virtual void setDebugMode(int debugMode);
	virtual void draw3dText(const btVector3& location, const char* textString);
	virtual void reportErrorWarning(const char* warningString);
	virtual int getDebugMode() const { return m_debugMode; }
	void draw();
};

