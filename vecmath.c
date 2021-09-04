#include <math.h>

typedef union {
	struct {
		float X, Y;
	};
	float E[2];
} vec2;

typedef union {
	struct {
		float X, Y, Z;
	};
	float E[3];
} vec3;

typedef union {
	struct {
		float X, Y, Z, W;
	};
	struct {
		vec3 XYZ;
		float Unsued0;
	};
	float E[4];
} vec4;

static vec3 Vec3(float X, float Y, float Z) {
	vec3 Res = {X, Y, Z};
	return Res;
}

static vec4 Vec4(float X, float Y, float Z, float W) {
	vec4 Res = {X, Y, Z, W};
	return Res;
}

static vec3 NegateVec3(vec3 In) {
	vec3 Res = {-In.X, -In.Y, -In.Z};
	return Res;
}

static vec3 AddVec3(vec3 V1, vec3 V2) {
	vec3 R = {V1.X + V2.X, V1.Y + V2.Y, V1.Z + V2.Z};
	return R;
}

static vec3 SubVec3(vec3 V1, vec3 V2) {
	vec3 R = {V1.X - V2.X, V1.Y - V2.Y, V1.Z - V2.Z};
	return R;
}

static vec3 ScaleVec3(vec3 V, float Scale) {
	vec3 R = {V.X*Scale, V.Y*Scale, V.Z*Scale};
	return R;
}

static vec3 DivVec3(vec3 V, float Scale) {
	vec3 R = {V.X/Scale, V.Y/Scale, V.Z/Scale};
	return R;
}

static vec3 AddScaledVec3(vec3 A, vec3 B, float Scale) {
	vec3 R = { A.X + B.X*Scale, A.Y + B.Y*Scale, A.Z + B.Z*Scale };
	return R;
}

static float LengthVec3(vec3 In) {
	return sqrt(In.X*In.X + In.Y*In.Y + In.Z*In.Z);
}

static vec3 NormalizeVec3(vec3 In) {
	float Value = LengthVec3(In);
	if(Value == 0.0f) return In;
	vec3 Res = {In.X/Value, In.Y/Value, In.Z/Value};
	return Res;
}

static float DotVec3(vec3 A, vec3 B) {
	float R = A.X*B.X + A.Y*B.Y + A.Z*B.Z;
	return R;
}

static vec3 CrossVec3(vec3 A, vec3 B) {
	// Rᵢ = Σⱼₖ εᵢⱼₖ AⱼBₖ
	vec3 R = {
		A.Y*B.Z - A.Z*B.Y,
		A.Z*B.X - A.X*B.Z,
		A.X*B.Y - A.Y*B.X
	};
	return R;
}

/* mat4 memory layout:
|  0  1  2  3 |
|  4  5  6  7 |
|  8  9 10 11 |
| 12 13 14 15 |
*/

typedef struct {
	float E[16];
} mat4;

static mat4 IdentityMat4 = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1,
};

static float SafeInv0(float Num) {
	float Result;
	if (Num == 0.0f)
		Result = 0.0f;
	else
		Result = 1 / Num;
	return Result;
}

static float Sin32(float Rad) {
	return sinf(Rad);
}

static float Cos32(float Rad) {
	return cosf(Rad);
}

static float Tan32(float Rad) {
	return tanf(Rad);
}

// Returns A * B
static mat4 MultMat4(mat4 A, mat4 B) {
	mat4 R = {0};
	for(int RowA = 0; RowA < 4; RowA++) {
		for(int ColB = 0; ColB < 4; ColB++) {
			int PosR = 4*RowA + ColB;
			for(int I = 0; I < 4; I++) {
				R.E[PosR] += A.E[4*RowA + I]*B.E[4*I + ColB];
			}
		}
	}
	return R;
}

static vec3 MultMat4Vec3(mat4 M, vec3 V) {
	vec3 R = {
		M.E[0]*V.X + M.E[1]*V.Z + M.E[2]*V.Z,
		M.E[4]*V.X + M.E[5]*V.Z + M.E[6]*V.Z,
		M.E[8]*V.X + M.E[9]*V.Z + M.E[10]*V.Z,
	};
	return R;
}

// Left is transposed
static float DualMultMat4(vec3 Left, mat4 Mat, vec3 Right) {
	vec3 T = {
		Left.X*Mat.E[0] + Left.Y*Mat.E[4] + Left.Z*Mat.E[8],
		Left.X*Mat.E[1] + Left.Y*Mat.E[5] + Left.Z*Mat.E[9],
		Left.X*Mat.E[2] + Left.Y*Mat.E[6] + Left.Z*Mat.E[10],
	};
	return DotVec3(T, Right);
}

static mat4 AddMat4(mat4 A, mat4 B) {
	mat4 R;
	for(int I = 0; I < 16; ++I) {
		R.E[I] = A.E[I] + B.E[I];
	}
	return R;
}

static mat4 ScalarMat4(mat4 M, float Scalar) {
	mat4 R;
	for(int I = 0; I < 16; ++I) {
		R.E[I] = M.E[I]*Scalar;
	}
	return R;
}

static mat4 TransposeMat4(mat4 M) {
	mat4 R;
	for(int Row = 0; Row < 4; Row++) {
		for(int Col = 0; Col < 4; Col++) {
			R.E[4*Row + Col] = M.E[4*Col + Row];
		}
	}
	return R;
}

// Returns a matrix M such that M * P results in P + T for any point P.
static mat4 TranslationMat4(vec3 T) {
	mat4 R = {
		1, 0, 0, T.X,
		0, 1, 0, T.Y,
		0, 0, 1, T.Z,
		0, 0, 0, 1,
	};
	return R;
}

static mat4 CrossProductMat4(vec3 A) {
	mat4 R = {
		   0, -A.Z,  A.Y, 0,
		 A.Z,    0, -A.X, 0,
		-A.Y, A.X,     0, 0,
		   0,   0,     0, 0,
	};
	return R;
}

static mat4 RotationAxisMat4(vec3 Axis, float Scale) {

	// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
	// v' = Rv
	// R = I + sin(t)*K + (1 - cos(t))*K^2
	float Rad = LengthVec3(Axis)*Scale;
	mat4 K = CrossProductMat4(NormalizeVec3(Axis));
	mat4 KSquare = MultMat4(K, K);
	mat4 R = AddMat4(IdentityMat4, ScalarMat4(K, Sin32(Rad)));
	R = AddMat4(R, ScalarMat4(KSquare, 1 - Cos32(Rad)));

	return R;
}

static mat4 RotationXMat4(float Rad) {
	float S = Sin32(Rad);
	float C = Cos32(Rad);
	mat4 R = {
		1,  0,  0, 0,
		0,  C, -S, 0,
		0,  S,  C, 0,
		0,  0,  0, 1,
	};
	return R;
}

static mat4 RotationYMat4(float Rad) {
	float S = Sin32(Rad);
	float C = Cos32(Rad);
	mat4 R = {
		 C, 0, S, 0,
		 0, 1, 0, 0,
		-S, 0, C, 0,
		 0, 0, 0, 1,
	};
	return R;
}

static mat4 RotationZMat4(float Rad) {
	float S = Sin32(Rad);
	float C = Cos32(Rad);
	mat4 R = {
		C, -S, 0, 0,
		S,  C, 0, 0,
		0,  0, 1, 0,
		0,  0, 0, 1,
	};
	return R;
}

static mat4 ScaleMat4(vec3 S) {
	mat4 R = {
		S.X,   0,   0,   0,
		  0, S.Y,   0,   0,
		  0,   0, S.Z,   0,
		  0,   0,   0,   1,
	};
	return R;
}

static mat4 InvScaleMat4(vec3 S) {
	float SX = SafeInv0(S.X);
	float SY = SafeInv0(S.Y);
	float SZ = SafeInv0(S.Z);
	mat4 R = {
		SX,   0,   0,   0,
		  0, SY,   0,   0,
		  0,   0, SZ,   0,
		  0,   0,   0,  1,
	};
	return R;
}

static mat4 TransformMat4(vec3 Position, mat4 Rotation, vec3 Scale) {
	mat4 Result = ScaleMat4(Scale);
	Result = MultMat4(Rotation, Result);
	Result = MultMat4(TranslationMat4(Position), Result);
	return Result;
}

static mat4 InvTransformMat4(vec3 Position, mat4 Rotation, vec3 Scale) {
	mat4 Result = TranslationMat4(NegateVec3(Position));
	Result = MultMat4(TransposeMat4(Rotation), Result);
	Result = MultMat4(InvScaleMat4(Scale), Result);
	return Result;
}

// Aspect is width over height
static mat4 ProjectionMatrix(float FOVY, float Aspect, float Near, float Far) {
	float TanY = Tan32(FOVY * 0.5f);
	float TanX = Aspect*TanY;
	mat4 R = {
		1/TanX,      0,                         0,                         0,
		     0, 1/TanY,                         0,                         0,
		     0,      0, (Far + Near)/(Near - Far),   2*Near*Far/(Near - Far),
		     0,      0,                        -1,                         0,
	};
	return R;
}