#include "FrustumCuller.h"

void frustum_culler::get_frustum_planes(glm::mat4 vp, glm::vec4* planes)
{
	vp = glm::transpose(vp); // creat row vectors
	planes[0] = glm::vec4(vp[3] + vp[0]); // left
	planes[1] = glm::vec4(vp[3] - vp[0]); // right
	planes[2] = glm::vec4(vp[3] + vp[1]); // bottom
	planes[3] = glm::vec4(vp[3] - vp[1]); // top
	planes[4] = glm::vec4(vp[3] + vp[2]); // near
	planes[5] = glm::vec4(vp[3] - vp[2]); // far
}

void frustum_culler::get_frustum_corners(glm::mat4 vp, glm::vec4* points)
{
	//corners in clip space
	const glm::vec4 corners[] = {
	glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1),
	glm::vec4(1, 1, -1, 1), glm::vec4(-1, 1, -1, 1),
	glm::vec4(-1, -1, 1, 1), glm::vec4(1, -1, 1, 1),
	glm::vec4(1, 1, 1, 1), glm::vec4(-1, 1, 1, 1) };

	// tranform into world space and divide by homogenous coordinate
	const glm::mat4 invVP = glm::inverse(vp);
	for (int i = 0; i < 8; i++) {
		const glm::vec4 p = invVP * corners[i];
		points[i] = p / p.w;
	}
}

bool frustum_culler::is_box_in_frustum(glm::vec4* planes, glm::vec4* corners, const bounding_box& b)
{
	// rejects if box is outside a frustum plane
	for (int i = 0; i < 6; i++) {
		int out = 0;
		out += (glm::dot(planes[i], glm::vec4(b.min_.x, b.min_.y, b.min_.z, 1.0f)) < 0.0f) ? 1 : 0;
		out += (glm::dot(planes[i], glm::vec4(b.max_.x, b.min_.y, b.min_.z, 1.0f)) < 0.0f) ? 1 : 0;
		out += (glm::dot(planes[i], glm::vec4(b.min_.x, b.max_.y, b.min_.z, 1.0f)) < 0.0f) ? 1 : 0;
		out += (glm::dot(planes[i], glm::vec4(b.max_.x, b.max_.y, b.min_.z, 1.0f)) < 0.0f) ? 1 : 0;
		out += (glm::dot(planes[i], glm::vec4(b.min_.x, b.min_.y, b.max_.z, 1.0f)) < 0.0f) ? 1 : 0;
		out += (glm::dot(planes[i], glm::vec4(b.max_.x, b.min_.y, b.max_.z, 1.0f)) < 0.0f) ? 1 : 0;
		out += (glm::dot(planes[i], glm::vec4(b.min_.x, b.max_.y, b.max_.z, 1.0f)) < 0.0f) ? 1 : 0;
		out += (glm::dot(planes[i], glm::vec4(b.max_.x, b.max_.y, b.max_.z, 1.0f)) < 0.0f) ? 1 : 0;
		if (out == 8) return false;
	}

	// rejects if box is outside frustum box
	int out = 0;
	out = 0; for (int i = 0; i < 8; i++) out += ((corners[i].x > b.max_.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((corners[i].x < b.min_.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((corners[i].y > b.max_.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((corners[i].y < b.min_.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((corners[i].z > b.max_.z) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((corners[i].z < b.min_.z) ? 1 : 0); if (out == 8) return false;

	return true;
}