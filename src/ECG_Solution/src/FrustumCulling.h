#pragma once

#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>
#include "LevelStructs.h"

namespace FrustumCulling
{
	/// @brief extracts 6 viewing planes from a view prjectio matrix of a right handed coordinate system
	/// https://fgiesen.wordpress.com/2012/08/31/frustum-planes-from-the-projection-matrix/
	/// https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
	/// @param vp is the view projection matrix, describing the view frustum
	/// @param planes is an array of 6 planes which contain the frustum planes after the method call
	void getFrustumPlanes(glm::mat4 vp, glm::vec4* planes)
	{
		vp = glm::transpose(vp); // creat row vectors
		planes[0] = glm::vec4(vp[3] + vp[0]); // left
		planes[1] = glm::vec4(vp[3] - vp[0]); // right
		planes[2] = glm::vec4(vp[3] + vp[1]); // bottom
		planes[3] = glm::vec4(vp[3] - vp[1]); // top
		planes[4] = glm::vec4(vp[3] + vp[2]); // near
		planes[5] = glm::vec4(vp[3] - vp[2]); // far
	}

	/// @brief extracts 8 corners of the viewing frustum from a view projection matrix
	/// @param vp is the view projection matrix, describing the view frustum
	/// @param points is an array of 8 vectors which contain the frustum corners after the method call
	void getFrustumCorners(glm::mat4 vp, glm::vec4* points)
	{
		//corners in clip space
		const glm::vec4 corners[] = {
		glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1),
		glm::vec4(1, 1, -1, 1), glm::vec4(-1, 1, -1, 1),
		glm::vec4(-1, -1, 1, 1), glm::vec4(1, -1, 1, 1),
		glm::vec4(1, 1, 1, 1), glm::vec4(-1, 1, 1, 1)};

		// tranform into world space and divide by homogenous coordinate
		const glm::mat4 invVP = glm::inverse(vp);
		for (int i = 0; i < 8; i++) {
			const glm::vec4 p = invVP * corners[i];
			points[i] = p / p.w;
		}
	}

	/// @brief 
	/// https://www.iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
	/// @param planes 
	/// @param corners 
	/// @param b 
	/// @return 
	bool isBoxInFrustum(glm::vec4* planes, glm::vec4* corners, const BoundingBox& b)
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
}