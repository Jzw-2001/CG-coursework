//#include <GL/gl3w.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include <iostream>
//#include <map>
//#include <vector>
//
//#include "camera.h"
//#include "error.h"
//#include "file.h"
//#include "shader.h"
//#include "obj_loader.h"
//
//using namespace std;
//
//glm::vec3 lightDirection = glm::vec3(0.1f, -0.81f, -0.61f);
//glm::vec3 lightPos = glm::vec3(2.f, 6.f, 7.f);
//SCamera Camera;
//const unsigned int WIDTH = 1024;
//const unsigned int HEIGHT = 768;
//
//struct MeshEntry {
//    GLuint VAO;
//    GLuint VBO;
//    unsigned int numIndices;
//
//    MeshEntry() : VAO(0), VBO(0), numIndices(0) {}
//};
//
//std::vector<MeshEntry> meshEntries;
//objl::Loader Loader;
//
//void setupMeshEntry(objl::Mesh& mesh) {
//    MeshEntry entry;
//    entry.numIndices = mesh.Indices.size();
//
//    glGenVertexArrays(1, &entry.VAO);
//    glGenBuffers(1, &entry.VBO);
//
//    glBindVertexArray(entry.VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, entry.VBO);
//
//    glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(objl::Vertex), &mesh.Vertices[0], GL_STATIC_DRAW);
//
//    // Vertex positions
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)0);
//
//    // Vertex normals
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, Normal));
//
//    // Vertex texture coords
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, TextureCoordinate));
//
//    glBindVertexArray(0);
//    meshEntries.push_back(entry);
//}
//
//void drawModel(unsigned int shaderProgram) {
//    for (auto& entry : meshEntries) {
//        glBindVertexArray(entry.VAO);
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
//        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
//        glDrawArrays(GL_TRIANGLES, 0, entry.numIndices);
//    }
//}
//
//int main(int argc, char** argv) {
//    glfwInit();
//    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lighting", NULL, NULL);
//    glfwMakeContextCurrent(window);
//
//    gl3wInit();
//    glEnable(GL_DEBUG_OUTPUT);
//    glDebugMessageCallback(DebugCallback, 0);
//
//    GLuint program = CompileShader("phong.vert", "phong.frag");
//    InitCamera(Camera, glm::vec3(0.0f, 0.0f, 5.0f));
//
//    // Load OBJ file
//    if (!Loader.LoadFile("path_to_your_obj_file.obj")) {
//        cerr << "Failed to load OBJ file!" << endl;
//        return -1;
//    }
//
//    for (auto& mesh : Loader.LoadedMeshes) {
//        setupMeshEntry(mesh);
//    }
//
//    glEnable(GL_DEPTH_TEST);
//
//    while (!glfwWindowShouldClose(window)) {
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glUseProgram(program);
//
//        // Set up your uniforms
//        drawModel(program);
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glfwDestroyWindow(window);
//    glfwTerminate();
//    return 0;
//}
