#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);
	void update();

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getPosition();

	glm::mat4 calculateViewMatrix();
	~Camera();

private:
	glm::vec3 position, worldUp, right, front, up;
	GLfloat yaw, pitch, moveSpeed, turnSpeed;
};
