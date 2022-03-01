#pragma once
#include "Utils.h"

class Camera
{
	public:
		// needed for rotating the camera in x and y direction
		float xRot = 0.0f;
		float yRot = 0.0f;

		// set ballarc camera radius to 6 units
		float radius = 6.0f;
		float rotationX = 3.1415926f/2; // 
		float rotationY = 0.0f;

		// clamps roation angle to 89.9 degrees
		float yClamp = 89.9f;

		int height;
		int width;

		// euler angles
		float yaw = -3.1415926f / 2;
		float pitch = 0.0f;

		glm::vec3 camPos;
		glm::vec3 camFront;
		glm::vec3 camUp;
		glm::vec3 camRight;
		glm::vec3 camTarget;
		glm::vec3 worldUp;

		// initialize matrices to be unit matrices
		glm::mat4 world = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float fov, int width, int height, float Znear, float Zfar);

		void BallArc(glm::vec3 &prev, GLFWwindow* window);

		void Zoom(float &yOffset);

		void Strafe(glm::vec3 &prev, GLFWwindow* window);

		glm::mat4 getViewProj();
		glm::mat4 getViewProjSkybox();

		void print();

		glm::mat4 lookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up);

		void updateCamVectors();

		glm::vec3 getUnitSphereVector(float x, float y);

		glm::vec3 getSphericalCoordinates(glm::vec3 cartesian);
		glm::vec3 getCartesianCoordinates(glm::vec3 spherical);

};