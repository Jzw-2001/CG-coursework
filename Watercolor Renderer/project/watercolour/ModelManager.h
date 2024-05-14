#pragma once

#include <vector>
#include <string>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION


#include "obj_loader.h"

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

	glm::vec3 position; // Position of the object
	glm::vec3 rotation; // Rotation of the object
	
	MeshEntry() : VAO(0), VBO(0), textureAmbientID(0), textureDiffuseID(0),
		textureSpecularID(0), textureSpecularHeighlightID(0), textureAlphaID(0),
		BumpMapID(0), textureNormalID(0), numIndices(0), Ns(0.0f), Ni(0.0f), d(1.f), illum(0) {
		Ka = glm::vec3(0.0f);
		Kd = glm::vec3(0.0f);
		Ks = glm::vec3(0.0f);
		position = glm::vec3(0.0f);
	}
};

class ModelManager {
private:
    GLuint loadTexture(const std::string& path);
	MeshEntry setupMeshEntry(const objl::Mesh& mesh, const glm::vec3& position);
	MeshEntry setupMeshEntry(const objl::Mesh& mesh, const glm::vec3& position, const glm::vec3& rotation);

public:
	std::vector<MeshEntry> meshEntries;
	std::vector<MeshEntry> waterMeshEntries;
	std::vector<MeshEntry> cloudMeshEntries;
    ModelManager();
    bool loadModel(const std::string& filePath, const glm::vec3& initialPosition, const glm::vec3& initialRotation);
	bool loadWaterModel(const std::string& filePath, const glm::vec3& initialPosition);
	bool loadCloudModel(const std::string& filePath, const glm::vec3& initialPosition, const glm::vec3& initialRotation);
	//void cleanup();
	void drawModel(GLuint shaderProgram, glm::vec3 lightPos, glm::vec3 cameraPosition, GLuint shadowMap, glm::mat4 lightSpaceMatrix);
	void drawModel(GLuint shaderProgram, glm::vec3 lightPos, glm::vec3 cameraPosition, GLuint shadowMap, glm::mat4 lightSpaceMatrix, glm::vec3 position, glm::vec3 rotation);
	void drawWaterModel(GLuint shaderProgram, glm::vec3 lightPos, glm::vec3 cameraPosition, GLuint shadowMap, glm::mat4 lightSpaceMatrix);
	void drawCloudModel(GLuint shaderProgram, glm::vec3 lightPos, glm::vec3 cameraPosition, GLuint shadowMap, glm::mat4 lightSpaceMatrix, glm::vec3 position, glm::vec3 rotation);
	//void drawCloudModel(GLuint shaderProgram, glm::vec3 lightPos, glm::vec3 cameraPosition, GLuint shadowMap, glm::mat4 lightSpaceMatrix);
	void drawShadowMap(GLuint shaderProgram, glm::mat4 lightSpaceMatrix);
};
