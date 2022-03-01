#include "CameraClass.h"


Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float fov, int width, int height, float Znear, float Zfar)
{
	std::cout << "setting up camera matrix..." << std::endl;

	camPos = pos;
	camFront = front;
	camUp = up;
	camTarget = camPos + camFront;
	worldUp = up;

	view = lookAt(camPos, camTarget, camUp);
	projection = glm::perspective(glm::radians(fov), (float)(width / height), Znear, Zfar);
	Camera::width = width;
	Camera::height = height;
}

void Camera::BallArc(glm::vec3 &prev, GLFWwindow* window)
{

	// get current mouse cursor position
	double xCur = 0, yCur = 0;
	glfwGetCursorPos(window, &xCur, &yCur);

	float dx = xCur - prev.x;
	float dy = prev.y - yCur;

	// determine mouse sensitivity
	float xScale = abs(dx) / width;
	float yScale = abs(dy) / height;
	float sensitivity = 5.0;
	glm::vec3 unit = glm::vec3(0.0f,0.0f,1.0f);
	float rad = glm::length(camPos);
	float theta = std::acos(camPos.z / rad);
	float phi = std::atan2(camPos.y, camPos.x);

	// move camera horizontal (on the y-axis)
	if (dx < 0)
	{
		yaw += xScale * sensitivity;
	}
	else if (dx > 0)
	{
		yaw -= (xScale * sensitivity);
	}

	// rotation in y direction (needed if rotation > 90 degrees)
	float rot = yScale * sensitivity;

	// move camera horizontal (on the x-axis)
	/*
	if (dy < 0)
	{
		std::cout << "rotate x-axis..." << std::endl;
		//if (yRot + rot > yClamp) rot = yClamp - yRot; //rotate upto nearly 90 degree

		pitch = std::min(std::max(pitch + rot, 0.0f), 3.1415926f / 2.0f);
	}
	else if (dy > 0)
	{
		std::cout << "rotate x-axis..." << std::endl;
		//if (yRot - rot < -yClamp) rot = yClamp + yRot; //rotate upto nearly 90 degree

		pitch = std::max(std::min(pitch - rot, 0.0f), -3.1415926f / 2.0f);
	}
	if (pitch > 3.1415926f / 2.0f)
		pitch = 3.1415926f / 2.0f;
	if (pitch < -3.1415926f / 2.0f)
		pitch = -3.1415926f / 2.0f;
		*/

	glm::vec3 dir;
	dir.x = cos(yaw) * cos(pitch);
	dir.y = sin(pitch);
	dir.z = sin(yaw) * cos(pitch);
	camFront = glm::normalize(dir);

	updateCamVectors();

	// update previous cursor position
	prev.x = xCur;
	prev.y = yCur;

	// prints cam matrix if needed
	//print();
}

void Camera::Zoom(float &yOffset)
{

	float sensitivity = 0.1f; // how sensitive the zoom is (between 0 and 1)
	float height = 0.0f;

	if (yOffset < 0) // scrolling in down direction
	{
		camPos -= camFront * sensitivity;
	}
	else if (yOffset > 0) // scrolling in up direction
	{
		camPos += camFront * sensitivity;
	}

	updateCamVectors();
}

void Camera::Strafe(glm::vec3 &prev, GLFWwindow* window) 
{

	double xCur = 0, yCur = 0;
	glfwGetCursorPos(window, &xCur, &yCur);

	// calculate the distance between last cursor position
	float dx = xCur - prev.x;
	float dy = prev.y - yCur;

	// determine mouse sensitivity
	float xScale = abs(dx) / width;
	float yScale = abs(dy) / height;
	float sensitivity = 0.05f;

	if (dx < 0) 
	{
		camPos -= camRight * sensitivity;
	}
	else if (dx > 0) 
	{
		camPos += camRight * sensitivity;
	}

	if (dy < 0) 
	{
		camPos -= camUp * sensitivity;
	}
	else if (dy > 0) 
	{
		camPos += camUp * sensitivity;
	}

	updateCamVectors();

	// update mouse cursor position
	prev.x = xCur;
	prev.y = yCur;
}

glm::mat4 Camera::getViewProj()
{
	// calculate and return view projection matrix
	return projection * lookAt(camPos, camPos + camFront, camUp);
}

glm::mat4 Camera::getViewProjSkybox()
{
	// calculate and return view projection matrix
	glm::mat4 V = lookAt(camPos, camPos + camFront, camUp);
	V = glm::mat4(glm::mat3(V)); // remove translation
	return projection * V;
}


// only for debug purposes
void Camera::print()
{
	glm::mat4 realview = lookAt(camPos, camTarget, camUp);
	std::cout << "m00=" << realview[0][0] << ", m01=" << realview[0][1] << ", m02=" << realview[0][2] << ", m03=" << realview[0][3] << std::endl;
	std::cout << "m10=" << realview[1][0] << ", m11=" << realview[1][1] << ", m12=" << realview[1][2] << ", m13=" << realview[1][3] << std::endl;
	std::cout << "m20=" << realview[2][0] << ", m21=" << realview[2][1] << ", m22=" << realview[2][2] << ", m23=" << realview[2][3] << std::endl;
	std::cout << "m30=" << realview[3][0] << ", m31=" << realview[3][1] << ", m32=" << realview[3][2] << ", m33=" << realview[3][3] << std::endl;
}

// takes an eye vector which is the camera position in world space,
// a target vector which is the position the camera should look at
// and an up vector which defines the up axis in the world
glm::mat4 Camera::lookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up)
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

void Camera::updateCamVectors()
{
	// calculate the new Front vector
	//glm::vec3 front;
	//front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	//front.y = sin(glm::radians(pitch));
	//front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	//camFront = glm::normalize(front);
	// 
	// also re-calculate the Right and Up vector
	camRight = glm::normalize(glm::cross(camFront, camUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	camUp = glm::normalize(glm::cross(camRight, camFront));
}

glm::vec3 Camera::getUnitSphereVector(float x, float y)
{
	float centerX = (width - 1) / 2.0f;
	float centerY = (height - 1) / 2.0f;
	int scale = std::min(width, height) - 1;
	float radius = 1.0f;

	// calculate the distance between last cursor position
	glm::vec3 Pt;
	Pt.x = 2 * (x - centerX) / (radius * scale);
	Pt.y = 2 * (centerY - y) / (radius * scale);
	float r = Pt.x * Pt.x + Pt.y * Pt.y;
	if (r > 1.0f)
	{
		float s = 1.0f / sqrt(r);
		Pt.x *= s;
		Pt.y *= s;
		Pt.z = 0.0f;
	}
	else
	{
		Pt.z = sqrt(1.0f - r);
	}

	return Pt;
}

glm::vec3 Camera::getSphericalCoordinates(glm::vec3 cartesian)
{

	float r = sqrt(cartesian.x * cartesian.x + cartesian.y * cartesian.y + cartesian.z * cartesian.z);
	float phi = std::atan2(cartesian.z/cartesian.x, cartesian.x);
	float theta = acos(cartesian.y/r);

	if (cartesian.x < 0.0f)
	{
		phi += 3.1415926f;
	}

	return glm::vec3(r,phi,theta);
}

glm::vec3 Camera::getCartesianCoordinates(glm::vec3 spherical)
{
	glm::vec3 result;

	result.x = spherical.x * sin(spherical.y) * cos(spherical.z);
	result.y = spherical.x * sin(spherical.y) * sin(spherical.z);
	result.z = spherical.x * cos(spherical.y);

	return result;
}

