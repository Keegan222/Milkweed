#version 330 core

in vec2 inPosition;
in vec2 inTextureCoords;

out vec2 textureCoords;

uniform mat4 cameraMatrix;

void main() {
	gl_Position = cameraMatrix * vec4(inPosition, 0.0, 1.0);
	textureCoords = inTextureCoords;
}