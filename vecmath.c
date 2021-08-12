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