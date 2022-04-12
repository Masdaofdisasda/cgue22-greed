#pragma once
#include "Utils.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "glm/gtx/euler_angles.hpp"

/* Camera Interface
* every camera has to give a view matrix and position vector for rendering and shading
*/
class CameraPositionerInterface
{
public:
	virtual ~CameraPositionerInterface() = default;
	virtual glm::mat4 getViewMatrix() const = 0;
	virtual glm::vec3 getPosition() const = 0;
	virtual glm::quat getOrientation() const = 0;
	virtual void update(double deltaSeconds, const glm::vec2& mousePos, bool mousePressed) = 0;
};

class Camera final
{
public:
	explicit Camera(CameraPositionerInterface& positioner)
		: positioner_(&positioner)
	{}

	Camera(const Camera&) = default;
	Camera& operator = (const Camera&) = default;

	glm::mat4 getViewMatrix() const { return positioner_->getViewMatrix(); }
	glm::vec3 getPosition() const { return positioner_->getPosition(); }
	glm::quat getOrientation() const { return positioner_->getOrientation(); }
	void setPositioner(CameraPositionerInterface* newPositioner) {
		positioner_ = newPositioner;
	}


private:
	CameraPositionerInterface* positioner_;
};

/* Camera with first person view
* view position can be moved in all 6 directions
* the player can only add accelertion to the camera, if the player stops pressing a key, the movement slowly stops
*/
class CameraPositioner_FirstPerson final : public CameraPositionerInterface
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
		float damping_ = 0.2f; // changes deceleration speed
		float maxSpeed_ = 10.0f; // clamps movement
		float fastCoef_ = .1f; // l-shift mode uses this



		glm::mat4 glmlookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up); // according to the book "real time rendering" (breaks camera)
		glm::mat4 glmlookAt2(glm::vec3 eye, glm::vec3 target, glm::vec3 up); //actual glm implementation
		CameraPositioner_FirstPerson(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
			: cameraPosition_(pos)
			, cameraOrientation_(glmlookAt2(pos, target, up))
			, up_(up)
		{}
		void setMovementState(KeyboardInputState input);
		void update(double deltaSeconds, const glm::vec2& mousePos, bool mousePressed) override;
		virtual glm::mat4 getViewMatrix() const override
		{
			const glm::mat4 t = glm::translate(glm::mat4(1.0f), -cameraPosition_);
			const glm::mat4 r = glm::mat4_cast(cameraOrientation_);
			return r * t;
		};
		virtual glm::vec3 getPosition() const override
		{
			return cameraPosition_;
		};
		void setPosition(const glm::vec3& pos);
		void resetMousePosition(const glm::vec2& p) { mousePos_ = p; };
		void setUpVector(const glm::vec3& up);
		inline void flookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);
		glm::quat getOrientation() const override {
			return cameraOrientation_;
		}

	private:
		glm::vec2 mousePos_ = glm::vec2(0);
		glm::vec3 cameraPosition_ = glm::vec3(0.0f, 10.0f, 10.0f);
		glm::quat cameraOrientation_ = glm::quat(glm::vec3(0));
		glm::vec3 moveSpeed_ = glm::vec3(0.0f);
		glm::vec3 up_ = glm::vec3(0.0f, 0.0f, 1.0f);

};

class CameraPositioner_Player final : public CameraPositionerInterface {
public:
	virtual glm::mat4 getViewMatrix() const override
	{
		const glm::mat4 t = glm::translate(glm::mat4(1.0f), -cameraPosition);
		const glm::mat4 r = glm::mat4_cast(cameraOrientation);
		return r * t;
	};
	virtual glm::vec3 getPosition() const override
	{
		return cameraPosition;
	};
	void setPosition(glm::vec3 pos);
	void update(double deltaSeconds, const glm::vec2& mousePos, bool mousePressed) override;
	glm::mat4 lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up);
	glm::quat getOrientation() const override {
		return cameraOrientation;
	}
private:
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat cameraOrientation = glm::quat(glm::vec3(0));
	glm::vec2 lastMousePos = glm::vec2(0);
	float mouseSpeed = 4.0f;

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
};