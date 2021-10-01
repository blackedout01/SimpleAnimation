#version 330

layout (location=0) in vec2 Position;

out vec3 CornerPosition;
// MatInvView = InvView * ScaleX(Aspect)
uniform mat4 MatInvView;
uniform float NearPlane;

void main() {
	vec4 FragPosition = vec4(Position, 0, 1);
	CornerPosition = (MatInvView * vec4(Position, 0, 0)).xyz + (MatInvView * vec4(0, 0, NearPlane, 0)).xyz;
	gl_Position = vec4(Position, 0, 1);
}