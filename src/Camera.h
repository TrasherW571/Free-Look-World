#pragma  once
#ifndef __Camera__
#define __Camera__

#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class MatrixStack;

class Camera
{
public:
	enum {
		ROTATE = 0,
		TRANSLATE,
		SCALE
	};
	
	Camera();
	virtual ~Camera();
	glm::vec3 getCamPosition() { return translations; }
	void setInitDistance(float z) { translations.z = std::abs(z); Front.z = translations.z - 1; }
	void setAspect(float a) { aspect = a; };
	void setRotationFactor(float f) { rfactor = f; };
	void setTranslationFactor(float f) { tfactor = f; };
	void setScaleFactor(float f) { sfactor = f; };
	void mouseMoved(float x, float y, float x2, float y2);
	void AdjustPosition(char direction);
	void applyProjectionMatrix(std::shared_ptr<MatrixStack> P) const;
	void applyViewMatrix(std::shared_ptr<MatrixStack> MV) const;
	
private:
	float aspect;
	float fovy;
	float znear;
	float zfar;
	glm::vec2 rotations;	// Represents rotations to our camera position
	glm::vec3 translations;	// Represents translations to our camera position
	glm::vec3 Front;		// Represents the point in front of our camera
	glm::vec3 Up;			// Represents which direction is up for our camera
	glm::vec2 mousePrev;
	int state;
	float rfactor;
	float tfactor;
	float sfactor;
	float speed;			// Represents our walking speed	
	float pitch;
	float pitchFactor;
	float yaw;
};

#endif
