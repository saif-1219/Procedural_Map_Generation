#pragma once

#include <GL/glut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {

public:
	//camera attributes
	glm::vec3 position;
	glm::vec3 worldUp;
	glm::vec3 right;
	glm::vec3 front;
	glm::vec3 up;

	//euler angles
	float yaw;
	float pitch;

	//camera options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;

	Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 Right = glm::vec3(1.0f, 0.0f, 0.0f), float Pitch = PITCH, float Yaw = YAW) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), zoom(ZOOM), mouseSensitivity(SENSITIVITY)
	{
		position = Position;
		up = UP;
		worldUp = UP;
		pitch = Pitch;
		yaw = Yaw;
		updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float Yaw, float Pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		position = glm::vec3(posX, posY, posZ);
		worldUp = glm::vec3(upX, upY, upZ);
		yaw = Yaw;
		pitch = Pitch;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	void processKeys(Camera_Movement direction, float deltaTime) 
	{
		float velocity = movementSpeed * deltaTime;
		if (direction == FORWARD)
			position += front * velocity;
		if (direction == BACKWARD)
			position -= front * velocity;
		if (direction == LEFT)
			position -= right * velocity;
		if (direction == RIGHT)
			position += right * velocity;
	}

	void processMouse(float xOffset, float yOffset)
	{
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		yaw += xOffset;
		pitch += yOffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		

		updateCameraVectors();
	}

private:
	void updateCameraVectors() {
		glm::vec3 direction;

		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		front = glm::normalize(direction);
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));

	}
};