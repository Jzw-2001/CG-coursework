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


void InitCamera(SCamera& in)
{
	in.Front = glm::vec3(0.0f, 0.0f, -1.0f);
	in.Position = glm::vec3(0.0f, 0.0f, 3.0f);
	in.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	in.WorldUp = in.Up;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	in.Yaw = -90.f;
	in.Pitch = 0.f;
}

float cam_dist = 2.f;

const float movementSpeed = .5f;
float MouseSensitivity = 1.f;



void MoveAndOrientCamera(SCamera& in, glm::vec3 target, float distance, float xoffset, float yoffset)
{
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