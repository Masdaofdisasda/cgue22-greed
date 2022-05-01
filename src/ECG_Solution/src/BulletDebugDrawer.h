#pragma once
#include <bullet/LinearMath/btIDebugDraw.h>
#include <vector>
#include "Utils.h"
#include "Program.h"

class bullet_debug_drawer final : public btIDebugDraw
{
private:
	int m_debug_mode_ = 0;
	std::vector <glm::vec3> vertices_;
	program program_;
	GLuint vbo_id_ = 0;
	GLuint vao_id_ = 0;
public:
	bullet_debug_drawer();
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
	void setDebugMode(int debugMode) override;
	void draw3dText(const btVector3& location, const char* textString) override;
	void reportErrorWarning(const char* warningString) override;
	int getDebugMode() const override { return m_debug_mode_; }
	void draw();
};

