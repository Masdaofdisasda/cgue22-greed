#include "Camera.h"

void CameraPositioner_FirstPerson::setMovementState(KeyboardInputState input)
{
	movement_.forward_ = input.pressingW;
	movement_.backward_ = input.pressingS;
	movement_.left_ = input.pressingA;
	movement_.right_ = input.pressingD;
	movement_.up_ = input.pressing1;
	movement_.down_ = input.pressing2;
	movement_.fastSpeed_ = input.pressingShift;

	if (input.pressingSpace)
		setUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
}

void CameraPositioner_FirstPerson::update(double deltaSeconds, const glm::vec2& mousePos, bool mousePressed) {
	// gets called once per loop

	// rotate camera by creating a quaternion from the mouse deltas
	if (mousePressed)
	{
		const glm::vec2 delta = mousePos - mousePos_;
		const glm::quat deltaQuat = glm::quat(glm::vec3(mouseSpeed_ * delta.y, mouseSpeed_ * delta.x, 0.0f));
		cameraOrientation_ = deltaQuat * cameraOrientation_;
		cameraOrientation_ = glm::normalize(cameraOrientation_);
		setUpVector(up_);
	}
	mousePos_ = mousePos;

	// translate camera by adding or substracting to the orthographic vectors
	const glm::mat4 v = glm::mat4_cast(cameraOrientation_);

	const glm::vec3 forward = -glm::vec3(v[0][2], v[1][2], v[2][2]);
	const glm::vec3 right = glm::vec3(v[0][0], v[1][0], v[2][0]);
	const glm::vec3 up = glm::cross(right, forward);

	glm::vec3 accel(0.0f);

	if (movement_.forward_) accel += forward;
	if (movement_.backward_) accel -= forward;

	if (movement_.left_) accel -= right;
	if (movement_.right_) accel += right;

	if (movement_.up_) accel += up;
	if (movement_.down_) accel -= up;

	if (movement_.fastSpeed_) accel *= fastCoef_;

	if (accel == glm::vec3(0))
	{
		// decelerate naturally according to the damping value
		moveSpeed_ -= moveSpeed_ * std::min((1.0f / damping_) * static_cast<float>(deltaSeconds), 1.0f);
	}
	else
	{
		// acceleration
		moveSpeed_ += accel * acceleration_ * static_cast<float>(deltaSeconds);
		const float maxSpeed = movement_.fastSpeed_ ? maxSpeed_ * fastCoef_ : maxSpeed_;
		if (glm::length(moveSpeed_) > maxSpeed) moveSpeed_ = glm::normalize(moveSpeed_) * maxSpeed;
	}

	cameraPosition_ += moveSpeed_ * static_cast<float>(deltaSeconds);
}

void CameraPositioner_FirstPerson::setPosition(const glm::vec3& pos)
{
	cameraPosition_ = pos;
}

void CameraPositioner_FirstPerson::setUpVector(const glm::vec3& up)
{
	const glm::mat4 view = getViewMatrix();
	const glm::vec3 dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
	cameraOrientation_ = glmlookAt2(cameraPosition_, cameraPosition_ + dir, up);
}

inline void CameraPositioner_FirstPerson::flookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up) {
	cameraPosition_ = pos;
	cameraOrientation_ = glmlookAt2(pos, target, up);
}

// takes an eye vector which is the camera position in world space,
// a target vector which is the position the camera should look at
// and an up vector which defines the up axis in the world
glm::mat4 CameraPositioner_FirstPerson::glmlookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up)
{
	// view is the diretion vector from the camera to the target
	glm::vec3 view = glm::normalize(target - eye);

	// right vector points to the right of the camera
	glm::vec3 right = glm::normalize(glm::cross(view, up));

	// up vector is normal to the view and right vectors 
	up = glm::cross(right, view);

	// R describe the orthonormal basis of the vectors,
	// the view direction is in -z 
	glm::mat4 R = glm::mat4(
		glm::vec4(right.x, up.x, -view.x, 0.0f),
		glm::vec4(right.y, up.y, -view.y, 0.0f),
		glm::vec4(right.z, up.z, -view.z, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	// the camera postion is translate to it's inverse position
	glm::mat4 T = glm::translate(glm::mat4(1.0f), eye * -1.0f);

	// the inverse of R is its transpose
	return glm::transpose(R) * T;
}

glm::mat4 CameraPositioner_FirstPerson::glmlookAt2(glm::vec3 pos, glm::vec3 target, glm::vec3 up)
{
	glm::vec3 zaxis = glm::normalize(pos - target);
	glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up), zaxis));
	glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	glm::mat4 translation; 
	translation[3][0] = -pos.x;
	translation[3][1] = -pos.y;
	translation[3][2] = -pos.z;
	glm::mat4 rotation;
	rotation[0][0] = xaxis.x; 
	rotation[1][0] = xaxis.y;
	rotation[2][0] = xaxis.z;
	rotation[0][1] = yaxis.x; 
	rotation[1][1] = yaxis.y;
	rotation[2][1] = yaxis.z;
	rotation[0][2] = zaxis.x; 
	rotation[1][2] = zaxis.y;
	rotation[2][2] = zaxis.z;

	return rotation * translation; 
}

