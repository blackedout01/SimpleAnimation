#version 330

layout (location=0) in vec3 Position;
layout (location=1) in vec2 Texture;
layout (location=2) in vec3 Normal;

out vec3 OutNormal;

uniform mat4 MatProj;
uniform mat4 MatView;
uniform mat4 MatModel;

void main() {
	gl_Position = MatProj*MatView*MatModel*vec4(Position, 1);
	OutNormal = (MatModel*vec4(Normal.xyz, 0.0)).xyz;
}