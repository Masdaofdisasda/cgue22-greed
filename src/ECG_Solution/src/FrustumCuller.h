#pragma once

//#include <GL\glew.h>
//#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>

void getFrustumPlanes(glm::mat4 vp, glm::vec4* planes)
{
	using glm::vec4;

	vp = glm::transpose(vp);
	planes[0] = vec4(vp[3] + vp[0]); // left
	planes[1] = vec4(vp[3] - vp[0]); // right
	planes[2] = vec4(vp[3] + vp[1]); // bottom
	planes[3] = vec4(vp[3] - vp[1]); // top
	planes[4] = vec4(vp[3] + vp[2]); // near
	planes[5] = vec4(vp[3] - vp[2]); // far
}

void getFrustumCorners(glm::mat4 vp, glm::vec4* points)
{
	using glm::vec4;

	const vec4 corners[] = {
	vec4(-1, -1, -1, 1), vec4(1, -1, -1, 1),
	vec4(1, 1, -1, 1), vec4(-1, 1, -1, 1),
	vec4(-1, -1, 1, 1), vec4(1, -1, 1, 1),
	vec4(1, 1, 1, 1), vec4(-1, 1, 1, 1)
	};
	const glm::mat4 invVP = glm::inverse(vp);
	for (int i = 0; i != 8; i++) {
		const vec4 q = invVP * corners[i];
		points[i] = q / q.w;
	}
}

/*bool isBoxInFrustum(glm::vec4* frPlanes,
	glm::vec4* frCorners,
	const BoundingBox& b)
{
	using glm::dot;
	using glm::vec4;
	for (int i = 0; i < 6; i++) {
		int r = 0;
		r += (dot(frPlanes[i],
			vec4(b.min_.x, b.min_.y, b.min_.z, 1.f)) < 0) ? 1 : 0;
		r += (dot(frPlanes[i],
			vec4(b.max_.x, b.min_.y, b.min_.z, 1.f)) < 0) ? 1 : 0;
		r += (dot(frPlanes[i],
			vec4(b.min_.x, b.max_.y, b.min_.z, 1.f)) < 0) ? 1 : 0;
		r += (dot(frPlanes[i],
			vec4(b.max_.x, b.max_.y, b.min_.z, 1.f)) < 0) ? 1 : 0;
		r += (dot(frPlanes[i],
			vec4(b.min_.x, b.min_.y, b.max_.z, 1.f)) < 0) ? 1 : 0;
		r += (dot(frPlanes[i],
			vec4(b.max_.x, b.min_.y, b.max_.z, 1.f)) < 0) ? 1 : 0;
		r += (dot(frPlanes[i],
			vec4(b.min_.x, b.max_.y, b.max_.z, 1.f)) < 0) ? 1 : 0;
		r += (dot(frPlanes[i],
			vec4(b.max_.x, b.max_.y, b.max_.z, 1.f)) < 0) ? 1 : 0;
		if (r == 8) return false;
	}*/