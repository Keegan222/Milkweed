#version 330 core

in vec2 textureCoords;
out vec4 color;

uniform sampler2D sampler;
uniform vec3 textColor;

void main() {
	vec4 textureColor = vec4(1.0, 1.0, 1.0, texture(sampler, textureCoords).r);
	color = vec4(textColor, 1.0) * textureColor;
}