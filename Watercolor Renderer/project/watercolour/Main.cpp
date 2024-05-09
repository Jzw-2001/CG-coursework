#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "shader.h"
#include "obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



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


struct MeshEntry {
	GLuint VAO;
	GLuint VBO;
	GLuint textureAmbientID;    // For ambient map (if used)
	GLuint textureDiffuseID;    // For diffuse map
	GLuint textureSpecularID;   // For specular map
	GLuint textureSpecularHeighlightID; // For specular highlight map (if used)
	GLuint textureAlphaID;      // For alpha map (if used)
	GLuint BumpMapID;           // For bump map (if used)
	GLuint textureNormalID;     // For normal map (if used)
	unsigned int numIndices;

	glm::vec3 Ka;  // Ambient color
	glm::vec3 Kd;  // Diffuse color
	glm::vec3 Ks;  // Specular color
	float Ns;      // Specular exponent
	float Ni;      // Optical density
	float d;       // Transparency
	int illum;     // Illumination model

	//MeshEntry() : VAO(0), VBO(0), textureDiffuseID(0), textureSpecularID(0), textureNormalID(0), numIndices(0), Ns(0.0f), Ni(0.0f), d(1.f), illum(0) {}
	MeshEntry() : VAO(0), VBO(0), textureAmbientID(0), textureDiffuseID(0),
		textureSpecularID(0), textureSpecularHeighlightID(0), textureAlphaID(0),
		BumpMapID(0), textureNormalID(0), numIndices(0), Ns(0.0f), Ni(0.0f), d(1.f), illum(0) {
		Ka = glm::vec3(0.0f);
		Kd = glm::vec3(0.0f);
		Ks = glm::vec3(0.0f);
	}
};


std::vector<MeshEntry> meshEntries;
objl::Loader Loader;



GLuint loadTexture(char const* path) {
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	// 添加前缀到文件路径
	std::string fullPath = "objects/";
	fullPath += path;  // 这里假设path是一个相对路径

	unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << fullPath << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}




void setupMeshEntry(objl::Mesh& mesh) {
	MeshEntry entry;
	entry.numIndices = mesh.Indices.size();

	glGenVertexArrays(1, &entry.VAO);
	glGenBuffers(1, &entry.VBO);

	glBindVertexArray(entry.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(objl::Vertex), &mesh.Vertices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)0);
	// Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, Normal));
	// Vertex texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, TextureCoordinate));

	glBindVertexArray(0);

	// Load textures if available
	/*if (!mesh.MeshMaterial.map_Kd.empty()) {
		entry.textureDiffuseID = loadTexture(mesh.MeshMaterial.map_Kd.c_str());
	}
	if (!mesh.MeshMaterial.map_Ks.empty()) {
		entry.textureSpecularID = loadTexture(mesh.MeshMaterial.map_Ks.c_str());
	}*/
	// map_Ka ambient texture map
	if (!mesh.MeshMaterial.map_Ka.empty()) {
		entry.textureAmbientID = loadTexture(mesh.MeshMaterial.map_Ka.c_str());
	}
	// map_Kd diffuse texture map
	if (!mesh.MeshMaterial.map_Kd.empty()) {
		entry.textureDiffuseID = loadTexture(mesh.MeshMaterial.map_Kd.c_str());
	}
	// map_Ks specular texture map
	if (!mesh.MeshMaterial.map_Ks.empty()) {
		entry.textureSpecularID = loadTexture(mesh.MeshMaterial.map_Ks.c_str());
	}
	// map_Ns specular highlight texture map
	if (!mesh.MeshMaterial.map_Ns.empty()) {
		entry.textureSpecularHeighlightID = loadTexture(mesh.MeshMaterial.map_Ns.c_str());
	}
	// map_d alpha texture map
	if (!mesh.MeshMaterial.map_d.empty()) {
		entry.textureAlphaID = loadTexture(mesh.MeshMaterial.map_d.c_str());
	}
	// map_bump bump texture map
	if (!mesh.MeshMaterial.map_bump.empty()) {
		entry.BumpMapID = loadTexture(mesh.MeshMaterial.map_bump.c_str());
	}

	// Copy material properties
	entry.Ka = glm::vec3(mesh.MeshMaterial.Ka.X, mesh.MeshMaterial.Ka.Y, mesh.MeshMaterial.Ka.Z);
	entry.Kd = glm::vec3(mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z);
	entry.Ks = glm::vec3(mesh.MeshMaterial.Ks.X, mesh.MeshMaterial.Ks.Y, mesh.MeshMaterial.Ks.Z);
	entry.Ns = mesh.MeshMaterial.Ns;
	entry.Ni = mesh.MeshMaterial.Ni;
	entry.d = mesh.MeshMaterial.d;
	entry.illum = mesh.MeshMaterial.illum;

	meshEntries.push_back(entry);
}




void drawModel(unsigned int shaderProgram) {
	// Activate shader program
	glUseProgram(shaderProgram);

	// Iterate through each mesh entry
	for (auto& entry : meshEntries) {
		glBindVertexArray(entry.VAO);

		// Set material properties
		glUniform3fv(glGetUniformLocation(shaderProgram, "material.ambient"), 1, glm::value_ptr(entry.Ka));
		glUniform3fv(glGetUniformLocation(shaderProgram, "material.diffuse"), 1, glm::value_ptr(entry.Kd));
		glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(entry.Ks));
		glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), entry.Ns);
		glUniform1f(glGetUniformLocation(shaderProgram, "material.transparency"), entry.d);

		// Set lighting properties
		glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(Camera.Position));
		glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); // Assuming white light


		// texture_amibient
		bool useTexture = false;
		if (entry.textureAmbientID != 0) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, entry.textureAmbientID);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture_ambient"), 1);
			useTexture = true;
		}
		// texture_diffuse
		if (entry.textureDiffuseID != 0) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, entry.textureDiffuseID);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 2);
			useTexture = true;
		}
		// texture_specular
		if (entry.textureSpecularID != 0) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, entry.textureSpecularID);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture_specular"), 3);
			useTexture = true;
		}
		// texture_specular_highlight
		if (entry.textureSpecularHeighlightID != 0) {
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, entry.textureSpecularHeighlightID);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture_specular_highlight"), 4);
			useTexture = true;
		}
		// texture_alpha
		if (entry.textureAlphaID != 0) {
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, entry.textureAlphaID);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture_alpha"), 5);
			glUniform1i(glGetUniformLocation(shaderProgram, "has_alpha"), true);
		}
		else {
			// uniform bool has_alpha = false
			glUniform1i(glGetUniformLocation(shaderProgram, "has_alpha"), false);
		}
		// texture_bump
		if (entry.BumpMapID != 0) {
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, entry.BumpMapID);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture_bump"), 6);
		}

		if (useTexture) {
			glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
		}
		else {
			glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), false);
		}
		// Model matrix setup
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Assuming all meshes are centered
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, entry.numIndices);
	}

	// Unbind VAO after drawing
	glBindVertexArray(0);
}





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

	GLuint program = CompileShader("phong.vert", "phong.frag");

	InitCamera(Camera);
	cam_dist = 5.f;
	MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);


	// create a mesh entry for each mesh in the obj file
	if (!Loader.LoadFile("objects/cat_quad_to_tri.obj")) {
		std::cerr << "Failed to load file" << std::endl;
		return -1;
	}

	for (auto& mesh : Loader.LoadedMeshes) {
		setupMeshEntry(mesh);
	}

	// create buffers respecting to the mesh entries
	for (auto& entry : meshEntries) {
		glBindVertexArray(entry.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);
	}

	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { .8f, .8f, .8f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUseProgram(program);

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100000.f);
		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		drawModel(program);

		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}