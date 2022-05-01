#pragma once

#include <glm\gtc\matrix_transform.hpp>
#include "LevelStructs.h"

class frustum_culler
{
public:

	static glm::mat4 cull_view_proj;
	static glm::vec4 frustum_planes[6];
	static glm::vec4 frustum_corners[8];
	static uint32_t models_loaded;
	static uint32_t models_visible;
	static double seconds_since_flush;

	/// @brief extracts 6 viewing planes from a view prjectio matrix of a right handed coordinate system
	/// https://fgiesen.wordpress.com/2012/08/31/frustum-planes-from-the-projection-matrix/
	/// https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
	/// @param vp is the view projection matrix, describing the view frustum
	/// @param planes is an array of 6 planes which contain the frustum planes after the method call
	static void get_frustum_planes(glm::mat4 vp, glm::vec4* planes);


	/// @brief extracts 8 corners of the viewing frustum from a view projection matrix
	/// @param vp is the view projection matrix, describing the view frustum
	/// @param points is an array of 8 vectors which contain the frustum corners after the method call
	static void get_frustum_corners(glm::mat4 vp, glm::vec4* points);


	/// @brief check if a given box is inside a given frustum
	/// https://www.iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
	/// @param planes are the 6 planes of the view frustum
	/// @param corners are the 8 corners of the view frustum
	/// @param b is the AABB of some mesh
	/// @return true if the box is in bounds, meaning its visible from the camera
	static bool is_box_in_frustum(glm::vec4* planes, glm::vec4* corners, const bounding_box& b);
};
