#version 420

out vec4 OutColor;

in vec3 CornerPosition;

uniform vec3 CameraPosition;

const vec3 InvLightDir = vec3(0.0f, 1.0f, 0.0f);

int Seed = 0x54948649;

int Rand() {
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	Seed = Seed ^ (Seed << 13) ^ (Seed >> 17) ^ (Seed << 5);
	return Seed;
}

float RandFloat() {
	return (Rand() & 65535)/65535.0;
}

vec2 RaySphereIntersection(vec3 Base, vec3 Dir, vec4 Desc) {
	// I  g: x = v + sw
	// II S: | x - M | = r
	// v = Base
	// w = Dir
	// M = Desc.xyz
	// I in II²:
	// p := wᵀ(v - M)/(wᵀw)
	// q := ((v - M)ᵀ(v - M) - r²)/(wᵀw)
	// s² + 2sp + q = 0
	// sₘᵢₙ = -p - sqrt(p² - q) if sₘᵢₙ >= 0

	vec3 BaseMinusMid = Base - Desc.xyz;
	float DirValueSq = dot(Dir, Dir);
	float P = dot(Dir, BaseMinusMid)/DirValueSq;
	float Q = (dot(BaseMinusMid, BaseMinusMid) - Desc.w)/DirValueSq;

	float R = P*P - Q;
	if(R < 0.0f)
		return vec2(-1.0f, -1.0f);
	float SqrtR = sqrt(R);
	return -P + vec2(-SqrtR, SqrtR);
}

// NOTE: Dir is assumed to be normalized.
vec2 RayNormSphereIntersection(vec3 Base, vec3 Dir, vec4 Desc) {
	vec3 BaseMinusMid = Base - Desc.xyz;
	float P = dot(Dir, BaseMinusMid);
	float Q = (dot(BaseMinusMid, BaseMinusMid) - Desc.a);

	float R = P*P - Q;
	if(R < 0.0f)
		vec2(-1.0f, -1.0f);
	float SqrtR = sqrt(R);
	return -P + vec2(-SqrtR, SqrtR);
}

// NOTE:
// Desc: center position, squared radius
// Color: color rgb, opaqueness value
// Attribs: inverse radius, roughness, refractive index, unused
struct sphere {
	vec4 Desc;
	vec4 Color;
	vec4 Attribs;
};

#define MaxSphereCount (6)
sphere Spheres[MaxSphereCount] = sphere[MaxSphereCount](
	//           x      y      z     r*r          cr    cg    cb    a           1/r   rough refr
	sphere(vec4( 0.0f, 0.7f,  0.0f, 0.16f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(2.50f, 1.0f, 1.3f, 0.0f)),
	sphere(vec4( 0.0f, 0.8f,  1.0f, 0.25f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec4(2.00f, 1.0f, 1.3f, 0.0f)),
	sphere(vec4( 1.0f, 0.8f,  0.2f, 0.25f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec4(2.00f, 1.0f, 1.3f, 0.0f)),
	sphere(vec4(-1.0f, 0.8f,  0.2f, 0.25f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec4(2.00f, 1.0f, 1.3f, 0.0f)),
	sphere(vec4( 0.0f, 1.5f, -2.0f, 1.00f), vec4(1.0f, 1.0f, 0.0f, 0.0f), vec4(1.00f, 0.0f, 1.46f, 0.0f)),
	sphere(vec4( 0.0f, -100,  0.0f, 10000), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(0.01f, 0.0f, 0.0f, 0.0f))
);

struct hit {
	float S;
	int Index;
};

// NOTE: Dir is assumed to be normalized.
hit ComputeClosestHitNorm(vec3 Base, vec3 Dir) {
	hit Hit;
	Hit.S = 10000000.0;
	Hit.Index = -1;
	for(int I = 0; I < MaxSphereCount; I++) {
		vec2 NewS = RayNormSphereIntersection(Base, Dir, Spheres[I].Desc);
		if(NewS.x > 0.0f && NewS.x < Hit.S) {
			Hit.Index = I;
			Hit.S = NewS.x;
		}
	}
	return Hit;
}

/*

NOTE:

computes mirror only color
Dir is assumed to be normalized

*/

vec4 ComputeColor(vec3 Base, vec3 Dir, int ExcludedIndex, int Depth) {
	vec4 Ret = vec4(1);
	
	for(int I = 0; I < Depth; I++) {
		hit Hit = ComputeClosestHitNorm(Base, Dir);
		if(Hit.Index >= 0) {
			
			vec3 Point = Base + Hit.S*Dir;
			vec3 Normal = normalize(Point - Spheres[Hit.Index].Desc.xyz);
			vec3 Reflection = reflect(Dir, Normal);
			//vec3 Refraction = refract(Dir, Normal, Attribs[Hit.Index]);
			//vec3 NewDir = Reflection + Roughness[Hit.Index]*normalize(vec3(RandFloat() - 0.5, RandFloat() - 0.5, RandFloat() - 0.5))
			//vec4 Result = ComputeColor(Point, Reflection, Hit.Index, Depth - 1);
			Ret *= Spheres[Hit.Index].Color;
			Base = Point;
			Dir = Reflection;
			ExcludedIndex = Hit.Index;
		} else {
			if(I != 0)  {
				Ret *= vec4(vec3(1)*max(0, dot(Dir, InvLightDir)), 1);
			}
			break;
		}
	}
	return Ret;
}

struct values {
	vec4 Color;
	vec4 ObjColor;
	vec3 Points[2];
	vec3 Dirs[2];
	float Factors[2];
	float Roughness;
	int State;
};

/*

NOTE:
computes the ray color including refraction and diffusion
Dir is assumed to be normalized

TODO:
refraction is incorrect if object is rough (ray into the object is not jittered)
refraction does not work for objects that contain other objects
mirrors and rough object use the same amount of subrays
leaf ray returns diffuse shaded object color which is incorrect (only lit if ray runs into light source)

*/

vec4 ComputeColorRefractDiffuse(vec3 Base, vec3 Dir) {

// NOTE: Max number of hits in tree depth
#define MaxDepth (5)

// NOTE: Number of spawned subrays (1/2 reflect, 1/2 refract)
#define MaxRoughIter (4*2)
	
	values Stack[MaxDepth];
	Stack[0].Points[1] = Base;
	Stack[0].Dirs[1] = Dir;
	Stack[0].Factors[1] = 1.0f;
	Stack[0].Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	Stack[0].ObjColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	Stack[0].State = MaxRoughIter - 1;
	Stack[0].Roughness = 0.0f;
	
	int Depth = 1;
	for(;;) {
		int TypeIndex = Stack[Depth - 1].State % 2;
		vec3 Point = Stack[Depth - 1].Points[TypeIndex];
		vec3 OutDir = Stack[Depth - 1].Dirs[TypeIndex];
		OutDir = normalize(OutDir + Stack[Depth - 1].Roughness*vec3(RandFloat() - 0.5, RandFloat() - 0.5, RandFloat() - 0.5));
		float Factor = Stack[Depth - 1].Factors[TypeIndex];
		++Stack[Depth - 1].State;
		
		hit Hit = ComputeClosestHitNorm(Point, OutDir);
		if(Factor > 0.0f && Hit.Index >= 0 && Depth < MaxDepth) {
			vec3 HitPointIn = Point + Hit.S*OutDir;
			vec3 HitNormalIn = (HitPointIn - Spheres[Hit.Index].Desc.xyz) * Spheres[Hit.Index].Attribs.x;
			vec3 RefractDir = refract(OutDir, HitNormalIn, 1.0f / Spheres[Hit.Index].Attribs.b);
			vec2 RefractS = RayNormSphereIntersection(HitPointIn, RefractDir, Spheres[Hit.Index].Desc);
			vec3 HitPointOut = HitPointIn + RefractS.y*RefractDir;
			vec3 HitNormalOut = (Spheres[Hit.Index].Desc.xyz - HitPointOut) * Spheres[Hit.Index].Attribs.x;
			
			Stack[Depth].Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			Stack[Depth].ObjColor = Spheres[Hit.Index].Color;
			Stack[Depth].Points[0] = HitPointIn;
			Stack[Depth].Points[1] = HitPointOut;
			Stack[Depth].Dirs[0] = reflect(OutDir, HitNormalIn);
			Stack[Depth].Dirs[1] = refract(RefractDir, HitNormalOut, Spheres[Hit.Index].Attribs.z);
			Stack[Depth].Factors[0] = Spheres[Hit.Index].Color.a;
			Stack[Depth].Factors[1] = 1.0f - Spheres[Hit.Index].Color.a;
			Stack[Depth].Roughness = Spheres[Hit.Index].Attribs.y;
			Stack[Depth].State = 0;
			
			++Depth;
		} else {
			// Factor zero, no hit or leaf depth reached: "return" diffuse shaded object color
			float DiffuseShade = Factor * max(0.0f, dot(OutDir, InvLightDir));
			Stack[Depth - 1].Color += vec4(DiffuseShade * Stack[Depth - 1].ObjColor.rgb, 0.0f);
			
			while(Stack[Depth - 1].State == MaxRoughIter) {
				if(Depth == 1) {
					return Stack[0].Color;
				}
				
				Factor = Stack[Depth - 2].Factors[(Stack[Depth - 2].State - 1) % 2];
				Stack[Depth - 2].Color += vec4((2.0f / MaxRoughIter) * Factor * Stack[Depth - 1].ObjColor.rgb * Stack[Depth - 1].Color.rgb, 0.0f);
				
				--Depth;
			}
		}
	}
	
#undef MaxDepth
#undef MaxRoughIter
}

void main() {
	// NOTE: Create different seed for every fragment
	Seed = floatBitsToInt(CornerPosition.x);
	Rand();
	Seed ^= floatBitsToInt(CornerPosition.y);
	Rand();
	Seed ^= floatBitsToInt(CornerPosition.z);
	Rand();
	
	vec3 RayDir = normalize(CornerPosition);
	//OutColor = ComputeColor(CameraPosition, RayDir, -1, 1000);
	OutColor = ComputeColorRefractDiffuse(CameraPosition, RayDir);
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