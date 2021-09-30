#include <math.h>

static float Sin32(float Rad) {
	return sinf(Rad);
}

static float Cos32(float Rad) {
	return cosf(Rad);
}

static float Tan32(float Rad) {
	return tanf(Rad);
}

static float Sqrt32(float R) {
	return sqrtf(R);
}

static float Atan232(float Y, float X) {
	return atan2f(Y, X);
}

static float Cbrt32(float R) {
	return cbrtf(R);
}

static float SafeInv0(float Num) {
	float Result;
	if (Num == 0.0f)
		Result = 0.0f;
	else
		Result = 1 / Num;
	return Result;
}

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

/* mat3 memory layout:
| 0 1 2 |
| 3 4 5 |
| 6 7 8 |
*/

typedef struct {
	float E[9];
} mat3;

static mat3 IdentityMat3 = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1,
};

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
		M.E[0]*V.X + M.E[1]*V.Y + M.E[2]*V.Z,
		M.E[4]*V.X + M.E[5]*V.Y + M.E[6]*V.Z,
		M.E[8]*V.X + M.E[9]*V.Y + M.E[10]*V.Z,
	};
	return R;
}

// Left is transposed
static float DualMultMat4Vec3(vec3 Left, mat4 Mat, vec3 Right) {
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

#if 0
static float DeterminantMat3(mat3 M) {
	// NOTE: See https://en.wikipedia.org/wiki/Rule_of_Sarrus
	// | 0 1 2 | 0 1
	// | 3 4 5 | 3 4
	// | 6 7 8 | 6 7
	float Result =
		+ M.E[0] * M.E[4] * M.E[8]
		+ M.E[1] * M.E[5] * M.E[6]
		+ M.E[2] * M.E[3] * M.E[7]
		- M.E[2] * M.E[4] * M.E[6]
		- M.E[0] * M.E[5] * M.E[6]
		- M.E[1] * M.E[3] * M.E[8]
	;
	return Result;
}
#endif

static mat4 FromColsMat3AsMat4(vec3 *Cols) {
	mat4 Result = {
		Cols[0].E[0], Cols[1].E[0], Cols[2].E[0], 0.0f,
		Cols[0].E[1], Cols[1].E[1], Cols[2].E[1], 0.0f,
		Cols[0].E[2], Cols[1].E[2], Cols[2].E[2], 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
	};
	return Result;
}

static float DeterminantMat3OfMat4(mat4 M) {
	// NOTE: See https://en.wikipedia.org/wiki/Rule_of_Sarrus
	// |  0  1  2  |  0  1
	// |  4  5  6  |  4  5
	// |  8  9 10  |  8  9
	float Result =
		+ M.E[0] * M.E[5] * M.E[10]
		+ M.E[1] * M.E[6] * M.E[8]
		+ M.E[2] * M.E[4] * M.E[9]
		- M.E[2] * M.E[5] * M.E[8]
		- M.E[0] * M.E[6] * M.E[9]
		- M.E[1] * M.E[4] * M.E[10]
	;
	return Result;
}

// Returns xi that satisfy xxx + Axx + Bx + C = 0 in R else xi = 0
static vec3 CubicRoots(float A, float B, float C) {
	
	// NOTE: See https://en.wikipedia.org/wiki/Cubic_equation
	
	// xxx + Axx + Bx + C = 0
	// D0 := AA - 3B
	// D1 := 2AAA - 9AB + 27C
	// Rd := D1*D1 - 4*D0*D0*D0
	// C := Cbrt((D1 + Sqrt(Rd))/2)
	
	vec3 Result;
	
	float D0 = A*A - 3*B;
	float D1 = 2*A*A*A - 9*A*B + 27*C;
	float Rd = D1*D1 - 4*D0*D0*D0;
	
	// NOTE: Compute CCC = (D1 + sqrt(Rd)) / 2;
	float CReal, CImag;
	if(Rd < 0.0f) {
		CReal = 0.5f*D1;
		CImag = 0.5f*Sqrt32(-Rd);
	} else {
		CReal = 0.5f*(D1 + Sqrt32(Rd));
		CImag = 0.0f;
	}
	
	// NOTE: Compute C = cbrt(CCC)
	// using pow(z, 1/n) = pow(r, 1/n)(cos(phi/n)+isin(phi/n))
	float Phi = Atan232(CImag, CReal);
	float R = Sqrt32(CReal*CReal + CImag*CImag);
	R = Cbrt32(R);
	CReal = R * Cos32(Phi/3);
	CImag = R * Sin32(Phi/3);
	
	// NOTE: xk = -1/3a(b + Pow(Xi, k)*C + D0/(Pow(Xi, k)*C)), k in {0, 1, 2} and Xi = -0.5 + Sqrt(-3/4)
	
	float XiReal = -0.5f;
	float XiImag = Sqrt32(0.75f);
	// NOTE: (a + bi)(a + bi) = (aa - bb) + (2ab)i
	float XiPowReal[] = { 1.0f, XiReal, XiReal*XiReal - XiImag*XiImag };
	float XiPowImag[] = { 0.0f, XiImag, 2*XiReal*XiImag };
	for(int I = 0; I < 3; ++I) {
		
		// NOTE: Multiply XiPow and C using
		// (a + bi)(c + di) = (ac - bd) + (ad + bc)i
		float XiPowCReal = XiPowReal[I]*CReal - XiPowImag[I]*CImag;
		float XiPowCImag = XiPowImag[I]*CReal + CImag*XiPowReal[I];
		
		// NOTE: Invert XiPowC using
		// 1/(a + bi) = (a/(aa + bb)) - (b/(aa + bb))i
		float LenSq = XiPowCReal*XiPowCReal + XiPowCImag*XiPowCImag;
		float InvXiPowCReal = XiPowCReal/LenSq;
		float InvXiPowCImag = -XiPowCImag/LenSq;
		
		float ResultImag = XiPowCImag + D0 * InvXiPowCImag;
		float Epsilon = 0.0000001f;
		if(ResultImag*ResultImag < Epsilon) {
			Result.E[I] = (A + XiPowCReal + D0 * InvXiPowCReal)/-3.0f;
		}
		else {
			Result.E[I] = 0.0f;
		}
	}
	
	return Result;
}

static vec3 EigenvaluesMat3OfMat4(mat4 M) {
	/*
		| a-λ   b   c  |
	det |  d   e-λ  f  | = 0
		|  g    h  i-λ |
	<=>
	+(a-λ)(e-λ)(i-λ) +bfg +cdh -c(e-λ)g -(a-λ)fh -bd(i-λ) = 0
	<=>
	λλλ + (-a-e-i)λλ + (+ae+ei+ai-cg-fn-bd)λ + (+ceg+afn+bdi-aei-bfg-cdh) = 0
	<=> (A := -a-e-i, B := +ae+ei+ai-cg-fn-bd, C := +ceg+afn+bdi-aei-bfg-cdh)
	λλλ + Aλλ + Bλ + C = 0
	*/
	
	float
		a = M.E[0],
		b = M.E[1],
		c = M.E[2],
		d = M.E[4],
		e = M.E[5],
		f = M.E[6],
		g = M.E[8],
		h = M.E[9],
		i = M.E[10]
	;
	
	float A = -a-e-i;
	float B = +a*e+e*i+a*i-c*g-f*h-b*d;
	float C = +c*e*g+a*f*h+b*d*i-a*e*i-b*f*g-c*d*h;
	
	vec3 Result = CubicRoots(A, B, C);
	return Result;
}

static mat4 EigenvectorsMat3OfMat4(mat4 M) {
	// Solve (M - λE)v = 0 for each λ
	
	vec3 Eigenvalues = EigenvaluesMat3OfMat4(M);
	
	vec3 Results[3] = {0};
	for(int I = 0; I < 3; ++I) {
		
		float Lamdba = Eigenvalues.E[I];
		if(Lamdba == 0.0f) {
			continue;
		} 
		
		vec3 Rows[] = {
			{ M.E[0] - Lamdba, M.E[1], M.E[2] },
			{ M.E[4], M.E[5] - Lamdba, M.E[6] },
			{ M.E[8], M.E[9], M.E[10] - Lamdba }
		};
		
		float Epsilon = 0.0000001f;
		vec3 C;
		int Indices[] = {0, 1, 0, 2, 1, 2};
		for(int J = 0; J < 3; ++J) {
			C = CrossVec3(Rows[Indices[2*J]], Rows[Indices[2*J+1]]);
			if(DotVec3(C, C) >= Epsilon) {
				Results[I] = C;
				break;
			}
		}
	}
	
	mat4 Result = FromColsMat3AsMat4(Results);
	return Result;
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