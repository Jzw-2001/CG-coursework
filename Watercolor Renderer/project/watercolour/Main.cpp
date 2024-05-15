#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "shader.h"
#include "ModelManager.h"
#include <direct.h>
#include "bitmap.h"


//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"



//light direction variable here
glm::vec3 lightDirection = glm::vec3(0.1f, -0.81f, -0.61f);
glm::vec3 lightPos = glm::vec3(2.f, 6.f, 7.f);

SCamera Camera;


#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1024
#define HEIGHT 768
#define SHADOW_WIDTH 2048
#define SHADOW_HEIGHT 2048


void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float x_offset = 0.f;
	float y_offset = 0.f;
	bool cam_changed = false;

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		x_offset += 1.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		x_offset -= 1.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		y_offset -= 1.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		y_offset += 1.f;
		cam_changed = true;
	}


	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		cam_dist -= 0.1f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		cam_dist += 0.1f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		lightDirection = Camera.Front;
		lightPos = Camera.Position;
	}

	if (cam_changed) {
		MoveAndOrientCamera(Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, x_offset * 0.1f, y_offset * 0.1f);
	}
}



void SizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}



//GLuint depthMapFBO;
//GLuint depthMap;
struct ShadowMap {
	GLuint depthMapFBO;
	GLuint Texture;
} shadowMap;


void initShadowMap() {
	glGenFramebuffers(1, &shadowMap.depthMapFBO);
	glGenTextures(1, &shadowMap.Texture);
	glBindTexture(GL_TEXTURE_2D, shadowMap.Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap.Texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


string GetCurrentWorkingDir() {
	char buff[FILENAME_MAX]; //create string buffer to hold path
	_getcwd(buff, FILENAME_MAX);
	string current_working_dir(buff);
	return current_working_dir;
}



void LoadModels(ModelManager &modelManager) {
	std::cout << "workingdir: " << GetCurrentWorkingDir().c_str() << std::endl;
	modelManager.loadWaterModel("objects/plane2.obj", glm::vec3(0, 0, -50));
	modelManager.loadModel("objects/boat2.obj", glm::vec3(0, 0, -10), glm::vec3(90, 0, 0));
	modelManager.loadCloudModel("objects/cloud.obj", glm::vec3(10, -10, 37), glm::vec3(90, 90, 0));
	modelManager.loadCloudModel("objects/cloud.obj", glm::vec3(30, 10, 42), glm::vec3(90, 90, 0));
	modelManager.loadCloudModel("objects/cloud.obj", glm::vec3(40, -15, 32), glm::vec3(90, 90, 0));
}



void CloudShift(ModelManager& modelManager, glm::vec3 direction, float speed, float deltaTime) {
	for (auto& entry : modelManager.cloudMeshEntries) {
		entry.position += direction * speed * deltaTime;
	}
}

void saveShadowMapToBitmap(unsigned int Texture, int w, int h) {
	float* pixelBuffer = (float*)malloc(sizeof(float) * w * h);// [] ;
	glBindTexture(GL_TEXTURE_2D, Texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixelBuffer);

	char* charBuffer = (char*)malloc(sizeof(unsigned char) * w * h * 3);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			charBuffer[(y * w * 3) + (x * 3) + 0] = pixelBuffer[(y * w) + x] * 255;
			charBuffer[(y * w * 3) + (x * 3) + 1] = pixelBuffer[(y * w) + x] * 255;
			charBuffer[(y * w * 3) + (x * 3) + 2] = pixelBuffer[(y * w) + x] * 255;
		}

	}

	BITMAPINFOHEADER infoHdr;
	infoHdr.biSize = 40;
	infoHdr.biWidth = w;
	infoHdr.biHeight = h;
	infoHdr.biPlanes = 1;
	infoHdr.biBitCount = 24;
	infoHdr.biCompression = 0;
	infoHdr.biSizeImage = sizeof(unsigned char) * w * h * 3;
	infoHdr.biXPelsPerMeter = 0;
	infoHdr.biYPelsPerMeter = 0;
	infoHdr.biClrUsed = 0;
	infoHdr.biClrImportant = 0;

	BITMAPFILEHEADER fileHdr;
	fileHdr.bfType = 19778;
	fileHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(unsigned char) * w * h * 3);
	fileHdr.bfReserved1 = 0;
	fileHdr.bfReserved2 = 0;
	fileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	savebitmap("shadowMap.bmp", (unsigned char*)charBuffer, &infoHdr, &fileHdr);

	free(charBuffer);
	free(pixelBuffer);
}


GLuint generateWhiteTexture() {
	unsigned char whitePixel[4] = { 255, 255, 255, 255 };
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	return textureID;
}




void processBoatMovement(GLFWwindow* window, std::vector<MeshEntry>& boatEntries, float deltaTime, glm::vec3& boatForward, glm::vec3& boatRight, float boatSpeed, float boatRotationSpeed) {
	// Up arrow to move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		for (auto& boatEntry : boatEntries) {
			boatEntry.position += boatForward * boatSpeed * deltaTime;
		}
	}

	// Right arrow to turn right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 newForward = glm::normalize(boatForward * glm::cos(-boatRotationSpeed * deltaTime) + boatRight * glm::sin(-boatRotationSpeed * deltaTime));
		glm::vec3 newRight = glm::normalize(glm::cross(glm::vec3(0, 0, 1), newForward));

		// Calculate the dot product and clamp it to the valid range for acos
		float dotProduct = glm::dot(boatForward, newForward);
		dotProduct = glm::clamp(dotProduct, -1.0f, 1.0f);

		// Calculate the angle between boatForward and newForward
		float angle = glm::degrees(glm::acos(dotProduct));

		// Determine the direction of rotation
		if (glm::dot(glm::cross(boatForward, newForward), glm::vec3(0, 0, 1)) < 0) {
			angle = -angle;
		}

		boatForward = newForward;
		boatRight = newRight;

		for (auto& boatEntry : boatEntries) {
			boatEntry.rotation.y += angle;
		}
	}

	// Left arrow to turn left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 newForward = glm::normalize(boatForward * glm::cos(boatRotationSpeed * deltaTime) + boatRight * glm::sin(boatRotationSpeed * deltaTime));
		glm::vec3 newRight = glm::normalize(glm::cross(glm::vec3(0, 0, 1), newForward));

		// Calculate the dot product and clamp it to the valid range for acos
		float dotProduct = glm::dot(boatForward, newForward);
		dotProduct = glm::clamp(dotProduct, -1.0f, 1.0f);

		// Calculate the angle between boatForward and newForward
		float angle = glm::degrees(glm::acos(dotProduct));

		// Determine the direction of rotation
		if (glm::dot(glm::cross(boatForward, newForward), glm::vec3(0, 0, 1)) < 0) {
			angle = -angle;
		}

		boatForward = newForward;
		boatRight = newRight;

		for (auto& boatEntry : boatEntries) {
			boatEntry.rotation.y += angle;
		}
	}
}






int main(int argc, char** argv)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lighting", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);
	gl3wInit();
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	

	initShadowMap();



	InitCamera(Camera);
	//cam_dist = 5.f;
	//MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), glm::vec3(-0.191527, -11.986, 73.5966).length(), -0.149106f, -9.25002f);
	
	/*Camera.Position: -0.191527 - 11.986 73.5966
		Camera.Front : 0.00256854 0.160743 - 0.986993
		Camera.Up : -0.000418314 0.986996 0.160742
		Camera.Right : -0.999997 0 - 0.00260238
		- 0.149106 - 9.25002*/
	/*Camera.Position = glm::vec3(-0.191527, -11.986, 73.5966);
	Camera.Front = glm::vec3(0.00256854, 0.160743, -0.986993);
	Camera.Up = glm::vec3(-0.000418314, 0.986996, 0.160742);
	Camera.Right = glm::vec3(-0.999997, 0.0, -0.00260238);
	Camera.Yaw = -0.149106;
	Camera.Pitch = -9.25002;
	Camera.WorldUp = glm::vec3(0, 0, 1);
	cam_dist = Camera.Position.length();*/

	ModelManager modelManager;
	LoadModels(modelManager);

	GLuint program = CompileShader("phong.vert", "phong.frag");
	GLuint programShadow = CompileShader("shadow.vert", "shadow.frag");
	GLuint programWater = CompileShader("water.vert", "water.frag");
	GLuint programCloud = CompileShader("cloud.vert", "cloud.frag");
	GLuint whiteTexture = generateWhiteTexture();

	int frame = 0;
	float currentTime = 0.f;
	float previousTime = 0.f;


	float cloudLoopDuration = 40.f;
	float currentCloudTime = 0.f;
	glm::vec3 cloudShiftDirection = glm::vec3(-1, 0, 0);
	float cloudSpeed = 1.f;

	//glfwSwapInterval(1); // 1 表示启用 V-Sync，0 表示禁用

	float boatSpeed;
	float boatRotationSpeed;
	glm::vec3 boatForward;
	glm::vec3 boatRight;

	boatForward = glm::normalize(glm::vec3(0, -1, 0));
	boatRight = glm::normalize(glm::vec3(-1, 0, 0));
	boatSpeed = 10.f;
	boatRotationSpeed = 2.0f;

	//MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), glm::vec3(-0.191527, -11.986, 73.5966).length(), -0.149106f, -9.25002f);

	lightPos = Camera.Position;
	lightDirection = Camera.Front;

	while (!glfwWindowShouldClose(window))
	{
		

		frame++;
		if (frame % 1000 == 0) {
			cout << "Camera.Position: " << Camera.Position.x << " " << Camera.Position.y << " " << Camera.Position.z << endl;
			cout << "Camera.Front: " << Camera.Front.x << " " << Camera.Front.y << " " << Camera.Front.z << endl;
			cout << "Camera.Up: " << Camera.Up.x << " " << Camera.Up.y << " " << Camera.Up.z << endl;
			cout << "Camera.Right: " << Camera.Right.x << " " << Camera.Right.y << " " << Camera.Right.z << endl;
			cout << Camera.Yaw << " " << Camera.Pitch << endl;
			/*cout << modelManager.meshEntries[0].position.x << " " << modelManager.meshEntries[0].position.y << " " << modelManager.meshEntries[0].position.z << endl;
			cout << modelManager.meshEntries[0].rotation.x << " " << modelManager.meshEntries[0].rotation.y << " " << modelManager.meshEntries[0].rotation.z << endl;
			cout << boatForward.x << " " << boatForward.y << " " << boatForward.z << endl;
			cout << boatRight.x << " " << boatRight.y << " " << boatRight.z << endl;*/
			//cout << "-------------------\n";
		}

		currentTime = glfwGetTime();
		//cout << currentTime << endl;
		float deltaTime = currentTime - previousTime;
		//cout << deltaTime << endl;
		//cout << "-------------------\n";
		previousTime = currentTime;
		// shadow mapping
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 700.5f;
		lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, lightPos + lightDirection, glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		modelManager.drawShadowMap(programShadow, lightSpaceMatrix);

		/*frame++;
		if (frame % 1000 == 0) {
			frame = 0;
			saveShadowMapToBitmap(shadowMap.Texture, SHADOW_WIDTH, SHADOW_HEIGHT);
		}*/

		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		
		// render boat

		processBoatMovement(window, modelManager.boatMeshEntries, deltaTime, boatForward, boatRight, boatSpeed, boatRotationSpeed);
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		static const GLfloat bgd[] = { .8f, .8f, .8f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



		glUseProgram(program);
		glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100000.f);
		glm::mat4 view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap.Texture);
		glUniform1i(glGetUniformLocation(program, "shadowMap"), 0);

		for (auto& entry : modelManager.boatMeshEntries) {
			glBindVertexArray(entry.VAO);
			glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);
		}

		modelManager.drawBoatModel(program, lightPos, Camera.Position, shadowMap.Texture, lightSpaceMatrix);


		// render water
		glUseProgram(programWater);
		glUniformMatrix4fv(glGetUniformLocation(programWater, "view"), 1, GL_FALSE, glm::value_ptr(view));	
		glUniformMatrix4fv(glGetUniformLocation(programWater, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(programWater, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glUniform1f(glGetUniformLocation(programWater, "time"), currentTime);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap.Texture);
		glUniform1i(glGetUniformLocation(programWater, "shadowMap"), 0);
		for (auto& entry : modelManager.waterMeshEntries) {
			glBindVertexArray(entry.VAO);
			glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);
		}

		modelManager.drawWaterModel(programWater, lightPos, Camera.Position, shadowMap.Texture, lightSpaceMatrix);


		// render cloud
		currentCloudTime += deltaTime;
		//cout << currentCloudTime << endl;
		if (currentCloudTime > cloudLoopDuration) {
			CloudShift(modelManager, cloudShiftDirection, cloudSpeed, - currentCloudTime);
			currentCloudTime = 0.f;
		}
		else {
			CloudShift(modelManager, cloudShiftDirection, cloudSpeed, deltaTime);
		}
		glDepthMask(GL_FALSE);
		glUseProgram(programCloud);
		glUniformMatrix4fv(glGetUniformLocation(programCloud, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(programCloud, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(programCloud, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glUniform1f(glGetUniformLocation(programCloud, "time"), currentTime);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, whiteTexture);
		glUniform1i(glGetUniformLocation(programCloud, "texture1"), 0);
		/*for (auto& entry : modelManager.cloudMeshEntries) {
			glBindVertexArray(entry.VAO);
			glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);
		}*/
		modelManager.drawCloudModel(programCloud, lightPos, Camera.Position, shadowMap.Texture, lightSpaceMatrix);
		glDepthMask(GL_TRUE);


		glfwSwapBuffers(window);
		glfwPollEvents();
		processKeyboard(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}