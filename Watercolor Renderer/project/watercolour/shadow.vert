#version 450 core

layout(location = 0) in vec4 vPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
	gl_Position = lightSpaceMatrix * model * vPos;
}