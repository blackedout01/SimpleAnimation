#version 330

layout (location=0) in vec3 Position;
layout (location=1) in vec3 Color;

out vec3 FragColor;

uniform mat4 MatPV;

void main() {
	FragColor = Color;
	gl_Position = MatPV * vec4(Position, 1.0);
}