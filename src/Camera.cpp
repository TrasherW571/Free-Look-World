#define _USE_MATH_DEFINES
#include <cmath> 
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "MatrixStack.h"

using namespace std;

Camera::Camera() :
	aspect(1.0f),
	fovy((float)(45.0*M_PI/180.0)),
	znear(0.1f),
	zfar(1000.0f),
	rotations(0.0, 0.0),
	translations(0.0f, 0.0f, 0.0f),
	Front(0.0f, 0.0f, -1.0f),
	Up(0.0f, 1.0f, 0.0f),		
	rfactor(0.01f),
	tfactor(0.001f),
	sfactor(0.005f),
	speed(0.1f),
	pitch(0.0f),
	yaw(-90.0f)
{
}

Camera::~Camera()
{
}

void Camera::mouseMoved(float x, float y, float x2, float y2)
{
	float sensitivity = 0.007f;	// How fast we move the camera
	glm::vec2 mouseCurr(x, y);
	mousePrev.x = x2;
	mousePrev.y = y2;
	glm::vec2 DeltaMouse;	// Represents the change from currentMouse - PrevMouse position
	DeltaMouse.x = mouseCurr.x - mousePrev.x;
	DeltaMouse.y = mousePrev.y - mouseCurr.y;
	DeltaMouse *= sensitivity;
	yaw += DeltaMouse.x;	// Yaw changes the x direction
	pitch += DeltaMouse.y;	// Pitch changes the y direction

	// Setting bounds on pitch
	if (pitch > 1.5f)
	{
		pitch = 1.5f;
	}
	if (pitch < -1.5f)
	{
		pitch = -1.5f;
	}
	// Adjusts the direction we are looking at
	glm::vec3 direction;
	direction.x = cos(pitch) * cos(yaw);
	direction.y = sin(pitch);
	direction.z = cos(pitch) * sin(yaw);
	Front = glm::normalize(direction);
	mousePrev = mouseCurr;
}

void Camera::AdjustPosition(char direction)
{
	// The commented out lines are if we allowed are camera to be able to move through the ground or fly through the air
	glm::vec3 result;
	switch (direction)
	{
	case 'w':
		translations.x += speed * Front.x;
		translations.z += speed * Front.z;
		//translations += speed * Front;
		break;
	case 's':
		translations.x -= speed * Front.x;
		translations.z -= speed * Front.z;
		//translations -= speed * Front;
		break;
	case 'a':
		result = glm::normalize(glm::cross(Front, Up));
		translations.x -= speed * result.x;
		translations.z -= speed * result.z;
		//translations -= glm::normalize(glm::cross(Front, Up)) * speed;
		break;
	case 'd':
		result = glm::normalize(glm::cross(Front, Up));
		translations.x += speed * result.x;
		translations.z += speed * result.z;
		//translations += glm::normalize(glm::cross(Front, Up)) * speed;
		break;
	default:
		break;
	}
}

void Camera::applyProjectionMatrix(std::shared_ptr<MatrixStack> P) const
{
	// Modify provided MatrixStack
	P->multMatrix(glm::perspective(fovy, aspect, znear, zfar));
}

// sets our camera position
void Camera::applyViewMatrix(std::shared_ptr<MatrixStack> MV) const
{
	glm::vec3 Target;		// Represents the point our camera is looking at
	Target = translations + Front;
	MV->multMatrix(glm::lookAt(translations, Target, Up));	// creates our view matrix and then multiplies it by our MV matrix
}
