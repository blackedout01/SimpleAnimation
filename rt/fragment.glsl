#version 420

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

// NOTE: Dir is assumed to be normalized.
float RayNormSphereIntersection(vec3 Base, vec3 Dir, vec3 Mid, float RadiusSquare) {
	vec3 BaseMinusMid = Base - Mid;
	float P = dot(Dir, BaseMinusMid);
	float Q = (dot(BaseMinusMid, BaseMinusMid) - RadiusSquare);

	float R = P*P - Q;
	if(R < 0) return -1.0;
	float SMin = -P - sqrt(R);
	
	return SMin;
}

#define MaxSphereCount (100)
float EarthRadius = 100;
vec4 Spheres[MaxSphereCount] = {
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 0, 0, 0.2f),
	vec4(0, 1, 0, 0.2f),
	vec4(1, 1, 0, 0.1f),
	vec4(0.5, 1, 0, 0.2f),
	vec4(0.5, 1, 5, 0.1f),
	vec4(0.5, 1.5, 0.5, 0.3f),
	vec4(0, 1, 1, 0.12f),
	vec4(0, -EarthRadius, 0, EarthRadius*EarthRadius),
};
vec4 Colors[MaxSphereCount] = {
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 1, 1),
	vec4(1, 1, 0.8, 1),
	vec4(1, 0.85, 1, 1),
	vec4(0.8, 1, 0.85, 1),
	vec4(1, 0.8, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
	vec4(0.8, 0.8, 0.8, 1),
	vec4(0.8, 1, 0.8, 1),
};
// roughness, refractive index
/*vec4 Attribs[MaxSphereCount] = {
	vec4(1, 1.3325, 0, 0),
	vec4(1, 1, 0, 0),
	vec4(1, 1, 0, 0),
	vec4(1, 1, 0, 0),
	vec4(1, 1, 0, 0),
	vec4(1, 1, 0, 0),
	vec4(1, 1, 0, 0),
	vec4(1, 1, 0, 0),
};*/
int seed = 0x54948649;
vec3 InvLightDir = vec3(0.0f, 1.0f, 0.0f);

int Rand() {
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	seed = seed ^ (seed << 13) ^ (seed >> 17) ^ (seed << 5);
	return seed;
}

float RandFloat() {
	return (Rand() & 65535)/65535.0;
}

struct hit {
	float S;
	int Index;
};

hit ComputeClosestHit(vec3 Base, vec3 Dir, int ExcludedIndex) {
	hit Hit;
	Hit.S = 10000000.0;
	Hit.Index = -1;
	for(int I = 0; I < MaxSphereCount; I++) {
		if(I == ExcludedIndex) continue;
		float NewS = RaySphereIntersection(Base, Dir, Spheres[I].xyz, Spheres[I].w);
		if(NewS > 0.0f && NewS < Hit.S) {
			Hit.Index = I;
			Hit.S = NewS;
		}
	}
	return Hit;
}

// NOTE: Dir is assumed to be normalized.
hit ComputeClosestHitNorm(vec3 Base, vec3 Dir, int ExcludedIndex) {
	hit Hit;
	Hit.S = 10000000.0;
	Hit.Index = -1;
	for(int I = 0; I < MaxSphereCount; I++) {
		if(I == ExcludedIndex) continue;
		float NewS = RayNormSphereIntersection(Base, Dir, Spheres[I].xyz, Spheres[I].w);
		if(NewS > 0.0f && NewS < Hit.S) {
			Hit.Index = I;
			Hit.S = NewS;
		}
	}
	return Hit;
}

bool IsAnyHit(vec3 Base, vec3 Dir, int ExcludedIndex) {
	for(int I = 0; I < MaxSphereCount; I++) {
		if(I == ExcludedIndex) continue;
		float S = RaySphereIntersection(Base, Dir, Spheres[I].xyz, Spheres[I].w);
		if(S > 0) {
			return true;
		}
	}
	return false;
}

// NOTE: Dir is assumed to be normalized.
vec4 ComputeColor(vec3 Base, vec3 Dir, int ExcludedIndex, int Depth) {
	vec4 Ret = vec4(1);
	
	for(int I = 0; I < Depth; I++) {
		hit Hit = ComputeClosestHitNorm(Base, Dir, ExcludedIndex);
		if(Hit.Index >= 0) {
			
			vec3 Point = Base + Hit.S*Dir;
			vec3 Normal = normalize(Point - Spheres[Hit.Index].xyz);
			vec3 Reflection = reflect(Dir, Normal);
			//vec3 Refraction = refract(Dir, Normal, Attribs[Hit.Index]);
			//vec3 NewDir = Reflection + Roughness[Hit.Index]*normalize(vec3(RandFloat() - 0.5, RandFloat() - 0.5, RandFloat() - 0.5))
			//vec4 Result = ComputeColor(Point, Reflection, Hit.Index, Depth - 1);
			Ret *= Colors[Hit.Index];
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

uniform int UniformSeed;
void main() {
	/*seed = floatBitsToInt(CornerPosition.x);
	Rand();
	seed ^= floatBitsToInt(CornerPosition.y);
	Rand();
	seed ^= floatBitsToInt(CornerPosition.z);
	Rand();
	seed ^= floatBitsToInt(CameraPosition.x);
	Rand();
	seed ^= floatBitsToInt(CameraPosition.y);
	Rand();
	seed ^= floatBitsToInt(CameraPosition.z);
	Rand();*/
	for(int I = 0; I < MaxSphereCount; I++) {
		Colors[I] = vec4(RandFloat(), RandFloat(), RandFloat(), 1);
		Spheres[I] = vec4(RandFloat()*10 - 5, RandFloat()*10 - 5, RandFloat()*10 - 5, RandFloat()*0.5);
	}
	//seed = UniformSeed;
	// Objekt
	// Color
	// Roughness, NIXRefraction

	#if 0
	hit FirstHit = ComputeClosestHit(CameraPosition, CornerPosition, -1);
	
	if(FirstHit.Index >= 0) {
		vec3 Point = CameraPosition + FirstHit.S*CornerPosition;
		vec3 Normal = normalize(Point - Spheres[FirstHit.Index].xyz);
		vec3 Reflection = normalize(reflect(normalize(CornerPosition), Normal));
		vec4 LightColor = vec4(0, 0, 0, 0);//Colors[FirstHit.Index];
		#define RayCount (100)
		for(int I = 0; I < RayCount; ++I) {
			vec3 Dir = Reflection + Roughness[FirstHit.Index]*normalize(vec3(RandFloat() - 0.5, RandFloat() - 0.5, RandFloat() - 0.5));
			hit Hit = ComputeClosestHit(Point, Dir, FirstHit.Index);
			if(Hit.Index != -1) {
//				LightColor += (1 / RayCount) * pow(Colors[Hit.Index], vec4(1.0/RayCount));
				vec3 Point2 = Point + Hit.S*Dir;
				//if(!IsAnyHit(Point2, InvLightDir, Hit.Index)) {
					LightColor += Colors[Hit.Index]*max(0.1, dot(normalize(Point2 - Spheres[Hit.Index].xyz), Dir));
				//}
			} else {
				LightColor += vec4(vec3(1)*dot(Dir, InvLightDir), 1);
			}
		}
		LightColor *= Colors[FirstHit.Index]/RayCount;

		//if(IsAnyHit(Point, InvLightDir)) {
			//LightIntensity = 0;
		//}
		//OutColor = Colors[FirstHit.Index]*max(vec4(0.1, 0.1, 0.1, 1), LightColor);
		OutColor = LightColor;
	} else {
		OutColor = vec4(1, 1, 1, 1);
	}
	#endif
	vec3 RayDir = normalize(CornerPosition);
	OutColor = ComputeColor(CameraPosition, RayDir, -1, 1000);
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