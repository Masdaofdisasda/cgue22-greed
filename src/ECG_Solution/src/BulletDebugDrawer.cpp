#include "BulletDebugDrawer.h"
#include <GL/glew.h>

bullet_debug_drawer::bullet_debug_drawer(){
	vertices_ = std::vector<glm::vec3>();

	Shader vertexShader("../assets/shaders/Testing/bulletDebug.vert");
	Shader fragmentShader("../assets/shaders/Testing/bulletDebug.frag");
	program_;
	program_.build_from(vertexShader, fragmentShader);
	glGenBuffers(1, &vbo_id_);
	glGenVertexArrays(1, &vao_id_);
}

void bullet_debug_drawer::draw() {

	if (vertices_.empty())
		return;

	// setup
	glBindVertexArray(vao_id_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(10);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// draw
	program_.use();
#ifdef _DEBUG
	glDrawArrays(GL_LINES, 0, vertices_.size());
#endif

	// cleanup
	
	vertices_.clear(); 
}

void bullet_debug_drawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	vertices_.push_back(glm::vec3(from.getX(), from.getY(), from.getZ()));
	vertices_.push_back(glm::vec3(to.getX(), to.getY(), to.getZ()));
}

void bullet_debug_drawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}

void bullet_debug_drawer::setDebugMode(int debugMode)
{
	m_debug_mode_ = debugMode;
}

void bullet_debug_drawer::draw3dText(const btVector3& location, const char* textString)
{
}

void bullet_debug_drawer::reportErrorWarning(const char* warningString)
{
	printf(warningString);
}
