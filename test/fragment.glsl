#version 330

in vec3 FragNormal;
in vec3 CameraDir;
in vec4 WorldPos;

out vec4 fragColor;

uniform vec4 LightDir;
uniform vec4 ModelColor;

uniform mat4 MatView;

#define MaxShadowCubeCount (128)
uniform mat4 ShadowCubes[MaxShadowCubeCount];
uniform int ShadowCubeCount;

#define CubeBorder (0.4999)
const vec3 ZeroVec3 = vec3(0, 0, 0);
const vec3 CubeBorderVec3 = vec3(CubeBorder, CubeBorder, CubeBorder);

bool RaySphereNotIntersection(vec3 Base, vec3 Dir, vec3 Mid, float RadiusSquare) {
	// Line: x = Base + S*Dir, S in R
	float S = (dot(Mid, Dir) - dot(Base, Dir))/dot(Dir, Dir);
	vec3 A = Base + S*Dir - Mid;
	return dot(A, A) > RadiusSquare;
}

float Intersect(vec3 Position, vec3 LightDir, int CubeIndex) {
	mat4 InvCube = ShadowCubes[CubeIndex];
	Position = (InvCube*vec4(Position, 1)).xyz;
	LightDir = (InvCube*vec4(LightDir, 0)).xyz;

	if(RaySphereNotIntersection(Position, LightDir, vec3(0, 0, 0), 0.75f)) return 1.0f;

	// Just test the 3 sides facing the light source.
	// Line l: x = Position + S * LightDir, S in R
	// S = (x - Position) / LightDir
	vec3 S = (sign(LightDir)*CubeBorder - Position)/LightDir;
	vec3 ProjX = abs(Position.yzx + S.xyz*LightDir.yzx); // "x" distance from origin on the projected plane.
	vec3 ProjY = abs(Position.zxy + S.xyz*LightDir.zxy); // "y" distance from origin on the projected plane.
	vec3 ProjMax = max(ProjX, ProjY); // max norm from origin on the projected plane.
	vec3 Results = 1.0 - step(ProjMax, CubeBorderVec3)*step(S, ZeroVec3);
	return Results.x*Results.y*Results.z;
}

void main() {
	vec3 Normal = normalize(FragNormal);
	float AmbientFac = 0.2;

	float Shadow = 1.0f;
	for(int I = 0; I < ShadowCubeCount; ++I) {
		Shadow *= Intersect(WorldPos.xyz, LightDir.xyz, I);
	}
	float DiffuseFac = dot(-LightDir.xyz, Normal)*0.6f * Shadow;
	int Shininess = 32;
	float SpecularFac = pow(max(0.0, dot(reflect(LightDir.xyz, Normal), -normalize(CameraDir))), Shininess) * Shadow;
	
	fragColor = vec4((SpecularFac + max(AmbientFac, DiffuseFac))*ModelColor.rgb, 1);

}