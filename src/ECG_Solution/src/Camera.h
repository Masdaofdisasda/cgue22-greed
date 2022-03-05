#pragma once
#include "Utils.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
	public:
		struct Movement
		{
			bool forward_ = false;
			bool backward_ = false;
			bool left_ = false;
			bool right_ = false;
			bool up_ = false;
			bool down_ = false;

			bool fastSpeed_ = false;
		} movement_;

		float mouseSpeed_ = 4.0f;
		float acceleration_ = 150.0f;
		float damping_ = 0.2f;
		float maxSpeed_ = 10.0f;
		float fastCoef_ = 10.0f;
		
		glm::mat4 glmlookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up);
		Camera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
			: cameraPosition_(pos)
			, cameraOrientation_(glmlookAt(pos, target, up))
			, up_(up)
		{}
		void update(double deltaSeconds, const glm::vec2& mousePos, bool mousePressed);
		glm::mat4 getViewMatrix();
		glm::vec3 getPosition();
		void setPosition(const glm::vec3& pos);
		void resetMousePosition(const glm::vec2& p) { mousePos_ = p; };
		void setUpVector(const glm::vec3& up);
		inline void lookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);

		/*_____________________________________________________________________________________________________*/

		glm::mat4 getViewMatrixSkybox();
	private:
		glm::vec2 mousePos_ = glm::vec2(0);
		glm::vec3 cameraPosition_ = glm::vec3(0.0f, 10.0f, 10.0f);
		glm::quat cameraOrientation_ = glm::quat(glm::vec3(0));
		glm::vec3 moveSpeed_ = glm::vec3(0.0f);
		glm::vec3 up_ = glm::vec3(0.0f, 0.0f, 1.0f);

};