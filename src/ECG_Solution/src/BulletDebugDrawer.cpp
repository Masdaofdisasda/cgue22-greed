#include "BulletDebugDrawer.h"
#include <GL/glew.h>

BulletDebugDrawer::BulletDebugDrawer(){
	vertices = std::vector<glm::vec3>();

	Shader* vertexShader = new Shader("assets/shaders/bulletDebug/bulletDebug.vert");
	Shader* fragmentShader = new Shader("assets/shaders/bulletDebug/bulletDebug.frag");
	program = new Program();
	program->buildFrom(*vertexShader, *fragmentShader);
	glGenBuffers(1, &vbo_ID);
	glGenVertexArrays(1, &vao_ID);
}

void BulletDebugDrawer::draw() {
	if (vertices.empty())
		return;

	// setup
	glBindVertexArray(vao_ID);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(10);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// draw
	program->Use();
	glDrawArrays(GL_LINES, 0, vertices.size());

	// cleanup
	
	vertices.clear();
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	vertices.push_back(glm::vec3(from.getX(), from.getY(), from.getZ()));
	vertices.push_back(glm::vec3(to.getX(), to.getY(), to.getZ()));
}

void BulletDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}

void BulletDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString)
{
	printf(warningString);
}
