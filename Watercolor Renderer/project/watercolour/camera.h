#pragma once

#include <stdio.h>
#include <glm/glm.hpp>

struct SCamera
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;

	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	const float MovementSpeed = 5.5f;
	float MouseSensitivity = 1.f;



};




float cam_dist = 5.f;

void InitCamera(SCamera& in) {
	in.Front = glm::vec3(0.00256854, 0.160743, -0.986993);
	in.Position = glm::vec3(-0.191527, -11.986, 73.5966);
	in.Up = glm::vec3(-0.000418314, 0.986996, 0.160742);
	in.WorldUp = glm::vec3(0, 1, 0);
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	in.Yaw = -0.149106;
	in.Pitch = -9.25002;
	std::cout << "init camera" << std::endl;
	std::cout << "init camera" << std::endl;
	std::cout << "init camera" << std::endl;
	std::cout << "init camera" << std::endl;
	std::cout << "init camera" << std::endl;
	std::cout << in.Position.length() << std::endl;
	cam_dist = 74.56f;
}

const float movementSpeed = .5f;
float MouseSensitivity = 1.f;



void MoveAndOrientCamera(SCamera& in, glm::vec3 target, float distance, float xoffset, float yoffset)
{
	/*std::cout << "MoveAndOrientCamera" << std::endl;
	std::cout << distance << std::endl;
	std::cout << xoffset << std::endl;
	std::cout << yoffset << std::endl;
	std::cout << "MoveAndOrientCameraEnd" << std::endl;*/
	// Calculate the new Yaw and Pitch angles based on the offsets
	float sensitivity = movementSpeed;  // Adjust this value to control the camera's rotation speed
	in.Yaw += xoffset * sensitivity;
	in.Pitch -= yoffset * sensitivity;

	// Clamp the Pitch to avoid flipping the camera
	float maxPitch = 89.0f;
	float minPitch = -89.0f;
	in.Pitch = glm::clamp(in.Pitch, minPitch, maxPitch);

	// Calculate the camera's position on the sphere
	glm::vec3 position;
	position.x = distance * cos(glm::radians(in.Pitch)) * sin(glm::radians(in.Yaw));
	position.y = distance * sin(glm::radians(in.Pitch));
	position.z = distance * cos(glm::radians(in.Pitch)) * cos(glm::radians(in.Yaw));
	in.Position = position + target;

	// Calculate the new front direction
	in.Front = glm::normalize(target - in.Position);

	// Calculate the new right direction (perpendicular to the front)
	glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
	in.Right = glm::normalize(glm::cross(worldUp, in.Front));

	// Calculate the new up direction (perpendicular to the front and right)
	in.Up = glm::normalize(glm::cross(in.Front, in.Right));
}