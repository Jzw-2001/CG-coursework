#include "ModelManager.h"
#include <glm/gtc/type_ptr.hpp>
#include "obj_loader.h"
#include "stb_image.h"

ModelManager::ModelManager() {}

GLuint ModelManager::loadTexture(const std::string& path) {
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	std::string fullPath = "objects/";
	fullPath += path;

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

MeshEntry ModelManager::setupMeshEntry(const objl::Mesh& mesh, const glm::vec3& position) {
	MeshEntry entry;
	entry.position = position;

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
	entry.position = position;
	entry.rotation = glm::vec3(0.0f);

	//meshEntries.push_back(entry);
	return entry;
}

MeshEntry ModelManager::setupMeshEntry(const objl::Mesh& mesh, const glm::vec3& position, const glm::vec3& rotation) {
	MeshEntry entry;
	entry.position = position;

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
	entry.position = position;
	entry.rotation = rotation;
	//meshEntries.push_back(entry);
	return entry;
}

bool ModelManager::loadModel(const std::string& filePath, const glm::vec3& pos, const glm::vec3& initialRotation) {
    objl::Loader Loader;
	Loader.LoadedIndices.clear();
	Loader.LoadedVertices.clear();
	Loader.LoadedMeshes.clear();

    if (!Loader.LoadFile(filePath)) {
        std::cerr << "Failed to load file: " << filePath << std::endl;
        return false;
    }
    for (auto& mesh : Loader.LoadedMeshes) {
        MeshEntry entry = setupMeshEntry(mesh, pos, initialRotation);
		meshEntries.push_back(entry);
    }
    return true;
}

bool ModelManager::loadWaterModel(const std::string& filePath, const glm::vec3& pos) {
	objl::Loader Loader;
	Loader.LoadedIndices.clear();
	Loader.LoadedVertices.clear();
	Loader.LoadedMeshes.clear();

	if (!Loader.LoadFile(filePath)) {
		std::cerr << "Failed to load file: " << filePath << std::endl;
		return false;
	}
	for (auto& mesh : Loader.LoadedMeshes) {
		MeshEntry entry = setupMeshEntry(mesh, pos);
		waterMeshEntries.push_back(entry);
	}
	return true;
}

void ModelManager::drawModel(GLuint shaderProgram, glm::vec3 lightPos, glm::vec3 cameraPosition, GLuint shadowMap, glm::mat4 lightSpaceMatrix) {

	//cout << "Drawing model" << endl;

	// Activate shader program
	glUseProgram(shaderProgram);

	// Set the light space matrix and the shadow map
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 0);

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
		glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPosition));
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
		//cout << "model before:" << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << endl;
		//printf("rotation: %.6f %.6f %.6f\n", entry.rotation.x, entry.rotation.y, entry.rotation.z);
		model = glm::translate(model, entry.position); // Assuming all meshes are centered
		// rotation
		model = glm::rotate(model, glm::radians(entry.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X axis
		model = glm::rotate(model, glm::radians(entry.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y axis
		model = glm::rotate(model, glm::radians(entry.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z axis
		//cout << "model after:" << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << endl;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, entry.numIndices);
	}

	// Unbind VAO after drawing
	glBindVertexArray(0);
}

void ModelManager::drawModel(GLuint shaderProgram, glm::vec3 lightPos, glm::vec3 cameraPosition, GLuint shadowMap, glm::mat4 lightSpaceMatrix, glm::vec3 position, glm::vec3 rotation) {

	//cout << "Drawing model" << endl;

	// Activate shader program
	glUseProgram(shaderProgram);

	// Set the light space matrix and the shadow map
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 0);

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
		glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPosition));
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
		//cout << "model before:" << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << endl;
		model = glm::rotate(model, glm::radians(entry.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X axis
		model = glm::rotate(model, glm::radians(entry.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y axis
		model = glm::rotate(model, glm::radians(entry.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z axis
		model = glm::translate(model, position); // Assuming all meshes are centered
		//cout << "model after:" << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << endl;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, entry.numIndices);
	}

	// Unbind VAO after drawing
	glBindVertexArray(0);
}

bool ModelManager::loadCloudModel(const std::string& filePath, const glm::vec3& initialPosition, const glm::vec3& initialRotation) {
	objl::Loader Loader;
	Loader.LoadedIndices.clear();
	Loader.LoadedVertices.clear();
	Loader.LoadedMeshes.clear();

	if (!Loader.LoadFile(filePath)) {
		std::cerr << "Failed to load file: " << filePath << std::endl;
		return false;
	}
	for (auto& mesh : Loader.LoadedMeshes) {
		MeshEntry entry = setupMeshEntry(mesh, initialPosition, initialRotation);
		cloudMeshEntries.push_back(entry);
	}
	return true;
}

void ModelManager::drawCloudModel(GLuint shaderProgram, glm::vec3 lightPos, glm::vec3 cameraPosition, GLuint shadowMap, glm::mat4 lightSpaceMatrix) {

	//cout << "Drawing model" << endl;

	// Activate shader program
	glUseProgram(shaderProgram);

	// Set the light space matrix and the shadow map
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 0);

	// Iterate through each mesh entry
	for (auto& entry : cloudMeshEntries) {
		glBindVertexArray(entry.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);

		// Set material properties
		glUniform3fv(glGetUniformLocation(shaderProgram, "material.ambient"), 1, glm::value_ptr(entry.Ka));
		glUniform3fv(glGetUniformLocation(shaderProgram, "material.diffuse"), 1, glm::value_ptr(entry.Kd));
		glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(entry.Ks));
		glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), entry.Ns);
		glUniform1f(glGetUniformLocation(shaderProgram, "material.transparency"), entry.d);

		// Set lighting properties
		glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPosition));
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
		//cout << "model before:" << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << endl;
		model = glm::rotate(model, glm::radians(entry.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X axis
		model = glm::rotate(model, glm::radians(entry.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y axis
		model = glm::rotate(model, glm::radians(entry.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z axis
		model = glm::translate(model, entry.position); // Assuming all meshes are centered
		//cout << "model after:" << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << endl;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, entry.numIndices);
	}

	// Unbind VAO after drawing
	glBindVertexArray(0);
}


void ModelManager::drawWaterModel(GLuint shaderProgram, glm::vec3 lightPos, glm::vec3 cameraPosition, GLuint shadowMap, glm::mat4 lightSpaceMatrix) {

	//cout << "Drawing model" << endl;

	// Activate shader program
	glUseProgram(shaderProgram);

	// Set the light space matrix and the shadow map
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 0);

	// Iterate through each mesh entry
	for (auto& entry : waterMeshEntries) {
		glBindVertexArray(entry.VAO);

		// Set material properties
		glUniform3fv(glGetUniformLocation(shaderProgram, "material.ambient"), 1, glm::value_ptr(entry.Ka));
		glUniform3fv(glGetUniformLocation(shaderProgram, "material.diffuse"), 1, glm::value_ptr(entry.Kd));
		glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(entry.Ks));
		glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), entry.Ns);
		glUniform1f(glGetUniformLocation(shaderProgram, "material.transparency"), entry.d);

		// Set lighting properties
		glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPosition));
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
		model = glm::translate(model, entry.position); // Assuming all meshes are centered
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, entry.numIndices);
	}

	// Unbind VAO after drawing
	glBindVertexArray(0);
}

void ModelManager::drawShadowMap(GLuint shaderProgram, glm::mat4 lightSpaceMatrix) {
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	for (auto& entry : meshEntries) {
		glBindVertexArray(entry.VAO);

		glm::mat4 model = glm::mat4(1.0f);
		//cout << "model before:" << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << endl;
		model = glm::rotate(model, glm::radians(entry.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X axis
		model = glm::rotate(model, glm::radians(entry.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y axis
		model = glm::rotate(model, glm::radians(entry.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z axis
		model = glm::translate(model, entry.position); // Assuming all meshes are centered
		
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, entry.numIndices);
	}

	for (auto& entry : cloudMeshEntries) {
		glBindVertexArray(entry.VAO);

		glm::mat4 model = glm::mat4(1.0f);
		//cout << "model before:" << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << endl;
		model = glm::rotate(model, glm::radians(entry.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X axis
		model = glm::rotate(model, glm::radians(entry.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y axis
		model = glm::rotate(model, glm::radians(entry.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z axis
		model = glm::translate(model, entry.position); // Assuming all meshes are centered

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, entry.numIndices);
	}

	//for (auto& entry : waterMeshEntries) {
	//	glBindVertexArray(entry.VAO);

	//	glm::mat4 model = glm::mat4(1.0f);
	//	//cout << "model before:" << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << endl;
	//	model = glm::rotate(model, glm::radians(entry.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X axis
	//	model = glm::rotate(model, glm::radians(entry.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y axis
	//	model = glm::rotate(model, glm::radians(entry.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z axis
	//	model = glm::translate(model, entry.position); // Assuming all meshes are centered
	//	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	//	glDrawArrays(GL_TRIANGLES, 0, entry.numIndices);
	//}
}