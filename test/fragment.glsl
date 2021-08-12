#version 330

in vec3 FragNormal;
in vec3 CameraDir;
in vec4 WorldPos;

out vec4 fragColor;

uniform vec4 LightDir;
uniform vec4 ModelColor;

uniform mat4 MatView;

#define ShadowCubeCount (128)
uniform mat4 ShadowCubes[ShadowCubeCount];

#define CubeBorder (0.4999)
const vec3 ZeroVec3 = vec3(0, 0, 0);
const vec3 CubeBorderVec3 = vec3(CubeBorder, CubeBorder, CubeBorder);

float Intersect(vec3 Position, vec3 LightDir, int CubeIndex) {
	mat4 InvCube = ShadowCubes[CubeIndex];
	Position = (InvCube*vec4(Position, 1)).xyz;
	LightDir = (InvCube*vec4(LightDir, 0)).xyz;
	vec3 InvLightDir = vec3(1.0f / LightDir.x, 1.0f / LightDir.y, 1.0f / LightDir.z);
	vec3 S;
	vec2 Proj;
	float Result = 1.0f;

	// Line l: x = Position + S * LightDir, S in R
	// S = (x - Position) / LightDir
	// 6 face intersections:

	S = (CubeBorder - Position)*InvLightDir;
	// Plane at x = CubeBorder
	//vec3 Proj1 = abs(Position.yzx + S.xyz*LightDir.yzx);
	//vec3 Proj2 = abs(Position.zxy + S.xyz*LightDir.zxy);
	//vec3 ProjMax = max(Proj1, Proj2);
	//vec3 Results = 1.0 - step(ProjMax, CubeBorderVec3) * step(S, ZeroVec3);
	//Result *= Results.x*Results.y*Results.z;
	//Result *= 1.0 - float(ProjMax.x < CubeBorder) * float(S.x < 0);
	//Result *= 1.0 - float(ProjMax.y < CubeBorder) * float(S.y < 0);
	//Result *= 1.0 - float(ProjMax.z < CubeBorder) * float(S.z < 0);

	Proj = abs(Position.yz + S.x*LightDir.yz);
	Result *= 1.0 - float(max(Proj.x, Proj.y) <= CubeBorder) * float(S.x < 0);
	// Plane at y = CubeBorder
	Proj = abs(Position.zx + S.y*LightDir.zx);
	Result *= 1.0 - float(max(Proj.x, Proj.y) <= CubeBorder) * float(S.y < 0);
	// Plane at z = CubeBorder
	Proj = abs(Position.xy + S.z*LightDir.xy);
	Result *= 1.0 - float(max(Proj.x, Proj.y) <= CubeBorder) * float(S.z < 0);

	S = (-CubeBorder - Position)*InvLightDir;
	//Proj1 = abs(Position.yzx + S.xyz*LightDir.yzx);
	//Proj2 = abs(Position.zxy + S.xyz*LightDir.zxy);
	//ProjMax = max(Proj1, Proj2);
	//Results *= 1.0 - step(ProjMax, CubeBorderVec3) * step(S, ZeroVec3);
	//Result *= Results.x*Results.y*Results.z;
	//Result *= 1.0 - float(ProjMax.x <= CubeBorder) * float(S.x < 0);
	//Result *= 1.0 - float(ProjMax.y <= CubeBorder) * float(S.y < 0);
	//Result *= 1.0 - float(ProjMax.z <= CubeBorder) * float(S.z < 0);
	//0.026749
	
	// Plane at x = -CubeBorder
	Proj = abs(Position.yz + S.x*LightDir.yz);
	Result *= 1.0 - float(max(Proj.x, Proj.y) <= CubeBorder) * float(S.x < 0);
	// Plane at y = -CubeBorder
	Proj = abs(Position.xz + S.y*LightDir.xz);
	Result *= 1.0 - float(max(Proj.x, Proj.y) <= CubeBorder) * float(S.y < 0);
	// Plane at z = -CubeBorder
	Proj = abs(Position.xy + S.z*LightDir.xy);
	Result *= 1.0 - float(max(Proj.x, Proj.y) <= CubeBorder) * float(S.z < 0);

	return Result;
}

void main() {
	vec3 Normal = normalize(FragNormal);
	float AmbientFac = 0.2;

	float Shadow = 1.0f;
	for(int I = 0; I < ShadowCubeCount; ++I) {
		Shadow *= float(Intersect(WorldPos.xyz, LightDir.xyz, I));
	}
	float DiffuseFac = dot(-LightDir.xyz, Normal)*0.6f * Shadow;
	int Shininess = 32;
	float SpecularFac = pow(max(0.0, dot(reflect(LightDir.xyz, Normal), -normalize(CameraDir))), Shininess) * Shadow;
	
	fragColor = vec4((SpecularFac + max(AmbientFac, DiffuseFac))*ModelColor.rgb, 1);

}