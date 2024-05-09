#ifndef MODEL_H
#define MODEL_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ModelManager.h"

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "shader.h"
#include "obj_loader.h"

#include "stb_image.h"

class Model {
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
public:
    Model(const std::string& path);

	void draw(GLuint shaderProgram, glm::vec3 lightPos, SCamera Camera);
	GLuint loadTexture(char const* path);

private:
    std::vector<MeshEntry> meshEntries;
    objl::Loader loader;

    void loadModel(const std::string& path);
    void setupMeshEntry(objl::Mesh& mesh);
};

#endif // MODEL_H
