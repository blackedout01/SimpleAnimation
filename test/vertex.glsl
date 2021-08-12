#version 330

layout (location=0) in vec3 Position;
layout (location=1) in vec2 Texture;
layout (location=2) in vec3 Normal;

out vec3 FragNormal;
out vec3 CameraDir;
out vec4 WorldPos;

uniform mat4 MatProj;
uniform mat4 MatView;
uniform mat4 MatModel;

void main() {
	WorldPos = MatModel*vec4(Position, 1);
	gl_Position = MatProj*MatView*WorldPos;
	vec4 CameraPos = inverse(MatView)*vec4(0, 0, 0, 1);
	CameraDir = normalize(WorldPos.xyz - CameraPos.xyz);
	FragNormal = normalize((MatModel*vec4(Normal.xyz, 0.0)).xyz);
}