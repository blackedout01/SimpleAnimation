#version 330

in vec3 OutNormal;

out vec4 fragColor;

uniform vec4 LightDir;
uniform vec4 ModelColor;

void main() {
	fragColor = vec4(max(0.2, dot(-LightDir.xyz, OutNormal))*ModelColor.rgb, 1);
}