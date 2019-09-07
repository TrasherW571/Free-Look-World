#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
using namespace std;

class Object
{
public:
	Object::Object(float tx, float ty, float tz, float rx, float ry, float rz, float a, float sx, float sy, float sz)
		:tFactor(tx,ty,tz), rFactor(rx,ry,rz), sFactor(sx,sy,sz)
	{
		angle = a;
	}
	glm::vec3 getTFactor() { return tFactor; }
	glm::vec3 getRFactor() { return rFactor; }
	glm::vec3 getSFactor() { return sFactor; }
	float getAngle() { return angle; }
private:
	float angle;
	glm::vec3 tFactor;
	glm::vec3 rFactor;
	glm::vec3 sFactor;
};