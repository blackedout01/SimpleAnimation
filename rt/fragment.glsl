#version 330

out vec4 OutColor;

in vec3 CornerPosition;

uniform vec3 CameraPosition;

float RaySphereIntersection(vec3 Base, vec3 Dir, vec3 Mid, float RadiusSquare) {
	// I  g: x = v + sw
	// II S: | x - M | = r
	// v = Base
	// w = Dir
	// M = Mid
	// I in II²:
	// p := wᵀ(v - M)/(wᵀw)
	// q := ((v - M)ᵀ(v - M) - r²)/(wᵀw)
	// s² + 2sp + q = 0
	// sₘᵢₙ = -p - sqrt(p² - q) if sₘᵢₙ >= 0

	vec3 BaseMinusMid = Base - Mid;
	float DirValueSq = dot(Dir, Dir);
	float P = dot(Dir, BaseMinusMid)/DirValueSq;
	float Q = (dot(BaseMinusMid, BaseMinusMid) - RadiusSquare)/DirValueSq;

	float R = P*P - Q;
	if(R < 0) return -1.0;
	float SMin = -P - sqrt(R);
	
	return SMin;
}

#define MaxSphereCount (3)
vec4 Spheres[MaxSphereCount];
vec4 Colors[MaxSphereCount];

void main() {

	vec3 InvLightDir = vec3(0.0f, 1.0f, 0.0f);

	Spheres[0] = vec4(1, 0, 0, 0.2f);
	Colors[0] = vec4(0, 0, 1, 1);
	Spheres[1] = vec4(0, 1, 0, 0.2f);
	Colors[1] = vec4(1, 1, 0, 1);
	float EarthRadius = 100;
	Spheres[2] = vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius);
	Colors[2] = vec4(1, 1, 0, 1);
	float Depth = 1000000000.0;
	int HitIndex = -1;
	for(int I = 0; I < MaxSphereCount; I++) {
		float NewDepth = RaySphereIntersection(CameraPosition, CornerPosition, Spheres[I].xyz, Spheres[I].w);
		if(NewDepth > 0 && NewDepth < Depth) {
			HitIndex = I;
			Depth = NewDepth;
		}
	}
	
	if(HitIndex >= 0) {
		vec3 Point = CameraPosition + Depth*CornerPosition;
		vec3 Normal = normalize(Point - Spheres[HitIndex].xyz);
		float LightIntensity = dot(InvLightDir, Normal);
		
		for(int I = 0; I < MaxSphereCount; I++) {
			float Depth = RaySphereIntersection(Point, InvLightDir, Spheres[I].xyz, Spheres[I].w);
			if(Depth > 0) {
				LightIntensity = 0;
				break;
			}
		}
		OutColor = Colors[HitIndex]*max(0.1, LightIntensity);
	} else {
		OutColor = vec4(1, 1, 1, 1);
	}
	/*if(RaySphereIntersection(CameraPosition, CornerPosition, vec3(1, 0, 0), 0.2f)) {
		OutColor = vec4(1, 0, 0, 1);
	}
	if(RaySphereIntersection(CameraPosition, CornerPosition, vec3(1, 1, 0), 0.2f)) {
		OutColor = vec4(1, 0, 1, 1);
	}
	if(RaySphereIntersection(CameraPosition, CornerPosition, vec3(1, 0, 1), 0.2f)) {
		OutColor = vec4(0, 1, 0, 1);
	}
	if(RaySphereIntersection(CameraPosition, CornerPosition, vec3(0, 1, 0), 0.2f)) {
		OutColor = vec4(0, 0, 1, 1);
	}
	if(RaySphereIntersection(CameraPosition, CornerPosition, vec3(0, 0, 1), 0.2f)) {
		OutColor = vec4(1, 1, 0, 1);
	}
	if(RaySphereIntersection(CameraPosition, CornerPosition, vec3(0, 1, 1), 0.2f)) {
		OutColor = vec4(0, 1, 1, 1);
	}*/

	//OutColor = vec4(vec3(RaySphereIntersection(CameraPosition, CornerPosition, vec3(1, 0, 0), 0.2f)), 1);
	
}

#if 0
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
#endif