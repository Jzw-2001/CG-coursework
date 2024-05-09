#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ModelManager.h"
#include "model.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "obj_loader.h"

#include "stb_image.h"



void Model::loadModel(const std::string& path) {
    if (!loader.LoadFile(path)) {
        std::cerr << "Failed to load file: " << path << std::endl;
        return;
    }

    for (auto& mesh : loader.LoadedMeshes) {
        setupMeshEntry(mesh);
    }
}

void Model::setupMeshEntry(objl::Mesh& mesh) {
    MeshEntry entry;
    entry.numIndices = mesh.Indices.size();

    glGenVertexArrays(1, &entry.VAO);
    glGenBuffers(1, &entry.VBO);

    glBindVertexArray(entry.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(objl::Vertex), &mesh.Vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, TextureCoordinate));

    glBindVertexArray(0);

    // Loading textures if they exist
    entry.textureAmbientID = !mesh.MeshMaterial.map_Ka.empty() ? loadTexture(mesh.MeshMaterial.map_Ka.c_str()) : 0;
    entry.textureDiffuseID = !mesh.MeshMaterial.map_Kd.empty() ? loadTexture(mesh.MeshMaterial.map_Kd.c_str()) : 0;
    entry.textureSpecularID = !mesh.MeshMaterial.map_Ks.empty() ? loadTexture(mesh.MeshMaterial.map_Ks.c_str()) : 0;
    entry.textureSpecularHeighlightID = !mesh.MeshMaterial.map_Ns.empty() ? loadTexture(mesh.MeshMaterial.map_Ns.c_str()) : 0;
    entry.textureAlphaID = !mesh.MeshMaterial.map_d.empty() ? loadTexture(mesh.MeshMaterial.map_d.c_str()) : 0;
    entry.BumpMapID = !mesh.MeshMaterial.map_bump.empty() ? loadTexture(mesh.MeshMaterial.map_bump.c_str()) : 0;

    entry.Ka = glm::vec3(mesh.MeshMaterial.Ka.X, mesh.MeshMaterial.Ka.Y, mesh.MeshMaterial.Ka.Z);
    entry.Kd = glm::vec3(mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z);
    entry.Ks = glm::vec3(mesh.MeshMaterial.Ks.X, mesh.MeshMaterial.Ks.Y, mesh.MeshMaterial.Ks.Z);
    entry.Ns = mesh.MeshMaterial.Ns;
    entry.Ni = mesh.MeshMaterial.Ni;
    entry.d = mesh.MeshMaterial.d;
    entry.illum = mesh.MeshMaterial.illum;

    meshEntries.push_back(entry);
}

//draw
void Model::draw(GLuint shaderProgram, glm::vec3 lightPos, SCamera Camera) {
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


		// Bind and set diffuse texture
		/*if (entry.textureDiffuseID != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, entry.textureDiffuseID);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse1"), 0);
		}*/
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

GLuint Model::loadTexture(char const* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    // 添加前缀到文件路径
    std::string fullPath = "objects/";
    fullPath += path;  // 这里假设path是一个相对路径

    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_RED;
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