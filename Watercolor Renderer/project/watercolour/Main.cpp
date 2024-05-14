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
		MoveAndOrientCamera(Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist * 0.1f, x_offset * 0.1f, y_offset * 0.1f);
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



//void RenderSceneToDepthMap() {
//	// Render to depth map
//	glm::mat4 lightProjection, lightView;
//	glm::mat4 lightSpaceMatrix;
//	float near_plane = 1.0f, far_plane = 7.5f;
//	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//	lightView = glm::lookAt(lightPos, lightPos + lightDirection, glm::vec3(0.0, 1.0, 0.0));
//	lightSpaceMatrix = lightProjection * lightView;
//	// Switch to frame buffer and draw scene as we normally would to color texture
//	glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
//	glClear(GL_DEPTH_BUFFER_BIT);
//	// Configure shader and matrices
//	GLuint program = CompileShader("shadow.vert", "shadow.frag");
//	glUseProgram(program);
//}



string GetCurrentWorkingDir() {
	char buff[FILENAME_MAX]; //create string buffer to hold path
	_getcwd(buff, FILENAME_MAX);
	string current_working_dir(buff);
	return current_working_dir;
}



void LoadModels(ModelManager &modelManager) {
	std::cout << "workingdir: " << GetCurrentWorkingDir().c_str() << std::endl;
	modelManager.loadWaterModel("objects/plane2.obj", glm::vec3(0, 0, -50));
	//modelManager.loadModel("objects/cat_quad_to_tri.obj", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	modelManager.loadModel("objects/boat2.obj", glm::vec3(0, 0, 0), glm::vec3(90, 0, 0));
	modelManager.loadCloudModel("objects/cloud.obj", glm::vec3(10, 0, 0));
	//modelManager.loadCloudModel("objects/boat2.obj", glm::vec3(10, 0, 0));

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
	cam_dist = 5.f;
	MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);



	ModelManager modelManager;
	LoadModels(modelManager);

	GLuint program = CompileShader("phong.vert", "phong.frag");
	GLuint programShadow = CompileShader("shadow.vert", "shadow.frag");
	GLuint programWater = CompileShader("water.vert", "water.frag");
	GLuint programCloud = CompileShader("cloud.vert", "cloud.frag");
	GLuint whiteTexture = generateWhiteTexture();

	int frame = 0;
	float currentTime = 0.f;
	while (!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();
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

		frame++;
		if (frame % 1000 == 0) {
			frame = 0;
			saveShadowMapToBitmap(shadowMap.Texture, SHADOW_WIDTH, SHADOW_HEIGHT);
		}

		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		
		// render boat
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

		for (auto& entry : modelManager.meshEntries) {
			glBindVertexArray(entry.VAO);
			glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);
		}

		modelManager.drawModel(program, lightPos, Camera.Position, shadowMap.Texture, lightSpaceMatrix);


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
		glUseProgram(programCloud);
		glUniformMatrix4fv(glGetUniformLocation(programCloud, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(programCloud, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(programCloud, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glUniform1f(glGetUniformLocation(programCloud, "time"), currentTime);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, whiteTexture);
		glUniform1i(glGetUniformLocation(programCloud, "texture1"), 0);
		for (auto& entry : modelManager.cloudMeshEntries) {
			glBindVertexArray(entry.VAO);
			glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);
		}
		modelManager.drawCloudModel(programCloud, lightPos, Camera.Position, shadowMap.Texture, lightSpaceMatrix, glm::vec3(10, 0, 0), glm::vec3(0, 0, 0));

		glfwSwapBuffers(window);
		glfwPollEvents();
		processKeyboard(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}