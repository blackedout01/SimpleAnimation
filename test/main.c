#include "../platform.c"

static GLuint ShaderProgram;
static GLint LocMatProj;
static GLint LocMatView;
static GLint LocMatModel;
static GLint LocLightDir;
static GLint LocModelColor;
static GLint LocShadowCubes;
static GLint LocShadowCubeCount;

static mesh CubeMesh;

typedef struct {
	mesh *Mesh;
	mat4 Mat;
	vec3 Position;
	vec4 Color;
} model;

static model Ground;
#define ShadowCubeCount (16)
static model Cubes[ShadowCubeCount];

void DrawModel(model *Model) {
	glBindVertexArray(Model->Mesh->VAO);
	glUniformMatrix4fv(LocMatModel, 1, GL_TRUE, Model->Mat.E);
	glUniform4fv(LocModelColor, 1, Model->Color.E);
	glDrawElements(GL_TRIANGLES, Model->Mesh->IndexCount, GL_UNSIGNED_INT, 0);
}

void MeshCreateBuffers(mesh *Mesh) {
	glGenVertexArrays(1, &Mesh->VAO);
	glBindVertexArray(Mesh->VAO);
	
	glGenBuffers(1, &Mesh->VBO);
	glGenBuffers(1, &Mesh->IBO);
	glBindBuffer(GL_ARRAY_BUFFER, Mesh->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh->IBO);
	glBufferData(GL_ARRAY_BUFFER, Mesh->VertexCount*sizeof(vertex), Mesh->Vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mesh->IndexCount*sizeof(int), Mesh->Indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(3*sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(5*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

float RandomFloat() {
	float Result = rand() / (float)RAND_MAX;
	return Result;
}

GLuint LineVAO;
GLuint LineVBO;
int MaxLineVertexCount = 100000;

typedef struct {
	vec3 Position;
	vec3 Color;
} line_vertex;

static vec3 Position;
static mat4 Rotation;
static vec3 Velocity;
static vec3 AngularVelocity;

#define TraceCapacity (2000)

typedef struct {
	vec3 Hist[TraceCapacity];
	int Count;
	int Index;
} trace;

static void TraceAdd(trace *Trace, vec3 Point) {
	Trace->Hist[Trace->Index++] = Point;
	if(Trace->Count < TraceCapacity)
		++Trace->Count;
	if(Trace->Index == TraceCapacity)
		Trace->Index = 0;
}

trace TopTrace;
trace OmegaTrace;

trace TraceX[3];

void Setup() {
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LINE_SMOOTH);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glLineWidth(3.0f);

	ShaderProgram = CompileShader("lines.vs", "lines.fs");

	glGenVertexArrays(1, &LineVAO);
	glBindVertexArray(LineVAO);
	
	glGenBuffers(1, &LineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
	glBufferData(GL_ARRAY_BUFFER, MaxLineVertexCount*sizeof(line_vertex), 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(line_vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(line_vertex), (void *)(3*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//Rotation = RotationXMat4(0.4f);
	Rotation = RotationXMat4(0.0f);
	//AngularVelocity = Vec3(0.9f, 0.0f, 0.9f);
	AngularVelocity = Vec3(0.3f, 0.9f, 0.0f);
	
#if 0
	CubeMesh = LoadOBJ("cube.obj");
	MeshCreateBuffers(&CubeMesh);

	Ground.Mesh = &CubeMesh;
	Ground.Color = Vec4(0.0f, 1.0f, 1.0f, 1.0f);
	vec3 Scale = {100, 0.1f, 100};
	Ground.Mat = MultMat4(ScaleMat4(Scale), IdentityMat4);
	vec3 Trans = {0, -5, 0};
	Ground.Mat = MultMat4(TranslationMat4(Trans), Ground.Mat);

	for(int i = 0; i < ArrayCount(Cubes); i++) {
		Cubes[i].Mesh = &CubeMesh;
		Cubes[i].Color = Vec4(RandomFloat(), RandomFloat(), RandomFloat(), 1);
		Cubes[i].Position = Vec3(RandomFloat()*10 - 5, RandomFloat()*10 - 5, RandomFloat()*10 - 5);
	}

	LocMatProj = glGetUniformLocation(ShaderProgram, "MatProj");
	LocMatView = glGetUniformLocation(ShaderProgram, "MatView");
	LocMatModel = glGetUniformLocation(ShaderProgram, "MatModel");
	LocLightDir = glGetUniformLocation(ShaderProgram, "LightDir");
	LocModelColor = glGetUniformLocation(ShaderProgram, "ModelColor"); 
	LocShadowCubes = glGetUniformLocation(ShaderProgram, "ShadowCubes");
	LocShadowCubeCount = glGetUniformLocation(ShaderProgram, "ShadowCubeCount");
#endif
}

static float Accu = 0.0f;

// 0 to 2pi
static float H = 0.0f;

// -pi/2 to pi/2
static float V = 0.0f;

static int LastMouseX = 0;
static int LastMouseY = 0;

static float Zoom = 6.0f;

static mat4 CylinderInertiaMatrix(float Mass, float Radius, float Height) {
	float IY = 0.5f * Mass * Radius * Radius;
	float IA = Mass * Height * Height / 12.0f + 0.5f * IY;
	mat4 Result = {
		IA, 0.0f, 0.0f, 0.0f,
		0.0f, IY, 0.0f, 0.0f,
		0.0f, 0.0f, IA, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};
	return Result;
}

static mat4 CylinderInvInertiaMatrix(float Mass, float Radius, float Height) {
	float IY = 0.5f * Mass * Radius * Radius;
	float IA = Mass * Height * Height / 12.0f + 0.5f * IY;
	mat4 Result = {
		1/IA, 0.0f, 0.0f, 0.0f,
		0.0f, 1/IY, 0.0f, 0.0f,
		0.0f, 0.0f, 1/IA, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};
	return Result;
}

static mat4 CuboidInertiaMatrix(float Mass, vec3 Size) {
	float SqX = Size.X * Size.X;
	float SqY = Size.Y * Size.Y;
	float SqZ = Size.Z * Size.Z;
	mat4 Result = {
		(SqY + SqZ) / 12.0f, 0.0f, 0.0f, 0.0f,
		0.0f, (SqX + SqZ) / 12.0f, 0.0f, 0.0f,
		0.0f, 0.0f, (SqX + SqY) / 12.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
	};
	return Result;
}

static mat4 CuboidInvInertiaMatrix(float Mass, vec3 Size) {
	float SqX = Size.X * Size.X;
	float SqY = Size.Y * Size.Y;
	float SqZ = Size.Z * Size.Z;
	mat4 Result = {
		12.0f / (SqY + SqZ), 0.0f, 0.0f, 0.0f,
		0.0f, 12.0f / (SqX + SqZ), 0.0f, 0.0f,
		0.0f, 0.0f,  12.0f / (SqX + SqY), 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
	};
	return Result;
}

static void PushLine(line_vertex *LineVertices, int *LineVertexCount, vec3 P0, vec3 P1, vec3 Color, mat4 MatM) {
	int Count = *LineVertexCount + 2;
	if(Count <= MaxLineVertexCount) {
		line_vertex V0 = { MultMat4Vec3(MatM, P0), Color };
		line_vertex V1 = { MultMat4Vec3(MatM, P1), Color };
		LineVertices[Count - 2] = V0;
		LineVertices[Count - 1] = V1;
		*LineVertexCount = Count;
	}
}

static void PushMat(line_vertex *LineVertices, int *LineVertexCount, mat4 Mat, mat4 MatM) {
	mat4 M = MultMat4(MatM, Mat);
	vec3 ZeroV3 = { 0.0f, 0.0f, 0.0f };
	vec3 OneXV3 = { 1.0f, 0.0f, 0.0f };
	vec3 OneYV3 = { 0.0f, 1.0f, 0.0f };
	vec3 OneZV3 = { 0.0f, 0.0f, 1.0f };
	PushLine(LineVertices, LineVertexCount, ZeroV3, OneXV3, OneXV3, M);
	PushLine(LineVertices, LineVertexCount, ZeroV3, OneYV3, OneZV3, M);
	PushLine(LineVertices, LineVertexCount, ZeroV3, OneZV3, OneYV3, M);
}

static void PushCuboid(line_vertex *LineVertices, int *LineVertexCount, vec3 P, vec3 Size, vec3 Color, mat4 MatM) {
	vec3 HalfSize = ScaleVec3(Size, 0.5f);
	vec3 P0s[] = {
		{ P.X - HalfSize.X, P.Y - HalfSize.Y, P.Z - HalfSize.Z },
		{ P.X - HalfSize.X, P.Y - HalfSize.Y, P.Z + HalfSize.Z },
		{ P.X + HalfSize.X, P.Y - HalfSize.Y, P.Z + HalfSize.Z },
		{ P.X + HalfSize.X, P.Y - HalfSize.Y, P.Z - HalfSize.Z },
	};
	
	vec3 P0p = P0s[3];
	for(int I = 0; I < 4; ++I) {
		vec3 P1p = P0p;
		P1p.Y += Size.Y;
		
		vec3 P0 = P0s[I];
		vec3 P1 = P0;
		P1.Y += Size.Y;
		
		PushLine(LineVertices, LineVertexCount, P0p, P1p, Color, MatM);
		PushLine(LineVertices, LineVertexCount, P0p, P0, Color, MatM);
		PushLine(LineVertices, LineVertexCount, P1p, P1, Color, MatM);
		
		P0p = P0;
	}
}

static void PushCircle(line_vertex *LineVertices, int *LineVertexCount, float Radius, int Resolution, vec3 Color, mat4 MatM) {
	vec3 P0p = { Radius, 0.0f, 0.0f };
	for(int I = 0; I < Resolution; ++I) {
		float Rad = (I + 1) * 6.28318530718f / Resolution;
		vec3 P0 = { Cos32(Rad) * Radius, 0.0f, Sin32(Rad) * Radius };
		PushLine(LineVertices, LineVertexCount, P0p, P0, Color, MatM);
		P0p = P0;
	}
}

static void PushCylinder(line_vertex *LineVertices, int* LineVertexCount, float Radius, float Height, int Resolution, vec3 Color, mat4 MatM) {
	float HalfHeight = 0.5f * Height;
	vec3 Z0 = { 0.0f, HalfHeight, 0.0f };
	vec3 Z1 = { 0.0f, -HalfHeight, 0.0f };
	vec3 P0p = { Radius, HalfHeight, 0.0f };
	for(int I = 0; I < Resolution; ++I) {
		float Rad = (I + 1) * 6.28318530718f / Resolution;
		vec3 P0 = { Cos32(Rad) * Radius, HalfHeight, Sin32(Rad) * Radius };
		PushLine(LineVertices, LineVertexCount, P0p, P0, Color, MatM);
		vec3 P1p = P0p;
		P1p.Y -= Height;
		vec3 P1 = P0;
		P1.Y -= Height;
		PushLine(LineVertices, LineVertexCount, P1p, P1, Color, MatM);
		PushLine(LineVertices, LineVertexCount, P0, P1, Color, MatM);
		//PushLine(LineVertices, LineVertexCount, Z0, P0, Color, MatM);
		//PushLine(LineVertices, LineVertexCount, Z1, P1, Color, MatM);
		P0p = P0;
	}
}

static void PushTrace(line_vertex *LineVertices, int* LineVertexCount, trace *Trace, vec3 Color) {
	for(int I = 0; I < Trace->Count - 1; ++I) {
		int Index0 = Trace->Index - I - 1;
		if(Index0 < 0)
			Index0 += TraceCapacity;
		int Index1 = Trace->Index - I - 2;
		if(Index1 < 0)
			Index1 += TraceCapacity;
		PushLine(LineVertices, LineVertexCount, Trace->Hist[Index0], Trace->Hist[Index1], Color, IdentityMat4);
	}
}

void Draw(context *Context) {
	if(Context->DeltaTime > 0.05f)
		return;
	
	float Aspect = Context->Width/(float)Context->Height;
	mat4 MatP = ProjectionMatrix(1.0f, Aspect, 0.1f, 100.0f);
	
	float MouseDX = (float)(Context->MouseX - LastMouseX);
	float MouseDY = (float)(Context->MouseY - LastMouseY);
	LastMouseX = Context->MouseX;
	LastMouseY = Context->MouseY;
	
	Zoom *= 1 + Context->ScrollDelta/10;
	vec3 T = {0, 0, -Zoom};
	
	if(Context->MouseDownLeft) {
		H += MouseDX*0.01f;
		V += MouseDY*0.01f;
	}
	
	Accu += Context->DeltaTime;
	
	mat4 MatV = IdentityMat4;
	MatV = MultMat4(RotationYMat4(H), MatV);
	MatV = MultMat4(RotationXMat4(V), MatV);
	MatV = MultMat4(TranslationMat4(T), MatV);
	
	mat4 MatPV = MultMat4(MatP, MatV);
	
	glClearColor(0.81f, 0.98f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(ShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "MatPV"), 1, GL_TRUE, MatPV.E);
	
	glBindVertexArray(LineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
	
	int LineVertexCount = 0;
	line_vertex *LineVertices = (line_vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	
	// dOmega/dt = inv(I) * (Tau - cross(Omega)*I*Omega)
	// Tau = cross(r, F) = 0

	float Radius = 0.125f;
	float Height = 4.06;
	
	vec3 Size = { 0.1f, 4.0f, 0.1f };
	//vec3 Size = { 4.0f, 4.0f, 4.0f };
	vec3 HalfSize = ScaleVec3(Size, 0.5f);

	mat4 InvI0 = CuboidInvInertiaMatrix(1.0f, Size);
	mat4 I0 = CuboidInertiaMatrix(1.0f, Size);
	mat4 InvI = MultMat4(MultMat4(Rotation, InvI0), TransposeMat4(Rotation));
	mat4 I = MultMat4(MultMat4(Rotation, I0), TransposeMat4(Rotation));
	
	mat4 EigenvectorsI = EigenvectorsMat3OfMat4(I);
	vec3 Iv0 = { EigenvectorsI.E[0], EigenvectorsI.E[4], EigenvectorsI.E[8] };
	vec3 Iv1 = { EigenvectorsI.E[1], EigenvectorsI.E[5], EigenvectorsI.E[9] };
	vec3 Iv2 = { EigenvectorsI.E[2], EigenvectorsI.E[6], EigenvectorsI.E[10] };
	
	#if 0
	vec3 AX = Vec3(I.E[0]*AngularVelocity.X,
	               I.E[4]*AngularVelocity.X,
				   I.E[8]*AngularVelocity.X);
	vec3 AY = Vec3(I.E[1]*AngularVelocity.Y,
	               I.E[5]*AngularVelocity.Y,
				   I.E[9]*AngularVelocity.Y);
	vec3 AZ = Vec3(I.E[2]*AngularVelocity.Z,
	               I.E[6]*AngularVelocity.Z,
				   I.E[10]*AngularVelocity.Z);
	#endif
	
	vec3 AX = Vec3(I.E[0],
	               I.E[4],
				   I.E[8]);
	vec3 AY = Vec3(I.E[1],
	               I.E[5],
				   I.E[9]);
	vec3 AZ = Vec3(I.E[2],
	               I.E[6],
				   I.E[10]);
	vec3 AXY = AddVec3(AX, AY);
	vec3 AXZ = AddVec3(AX, AZ);
	vec3 AYZ = AddVec3(AY, AZ);
	
	//AX = Iv0;
	//AY = Iv1;
	//AZ = Iv2;
	
	TraceAdd(TraceX + 0, AX);
	TraceAdd(TraceX + 1, AY);
	TraceAdd(TraceX + 2, AZ);
	
	mat4 X = {
		I.E[0]*AngularVelocity.X, I.E[1]*AngularVelocity.Y, I.E[2]* AngularVelocity.Z, 0.0f,
		I.E[4]*AngularVelocity.X, I.E[5]*AngularVelocity.Y, I.E[6]* AngularVelocity.Z, 0.0f,
		I.E[8]*AngularVelocity.X, I.E[9]*AngularVelocity.Y, I.E[10]*AngularVelocity.Z, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};
	
	vec3 L = MultMat4Vec3(I, AngularVelocity);
	vec3 T2 = MultMat4Vec3(CrossProductMat4(AngularVelocity), L);
	vec3 dOmega = MultMat4Vec3(InvI, NegateVec3(T2));
	
	AngularVelocity = AddScaledVec3(AngularVelocity, dOmega, Context->DeltaTime);
	
	PushMat(LineVertices, &LineVertexCount, IdentityMat4, IdentityMat4);
	
	mat4 dR = RotationAxisMat4(AngularVelocity, Context->DeltaTime);
	Rotation = MultMat4(dR, Rotation);
	//PushMat(LineVertices, &LineVertexCount, Rotation, IdentityMat4);
	//PushMat(LineVertices, &LineVertexCount, I0, IdentityMat4);
	PushMat(LineVertices, &LineVertexCount, I, IdentityMat4);
	//PushMat(LineVertices, &LineVertexCount, X, IdentityMat4);
	
	PushTrace(LineVertices, &LineVertexCount, TraceX + 0, Vec3(1.0f, 0.0f, 0.0f));
	PushTrace(LineVertices, &LineVertexCount, TraceX + 1, Vec3(0.0f, 0.0f, 1.0f));
	PushTrace(LineVertices, &LineVertexCount, TraceX + 2, Vec3(0.0f, 1.0f, 0.0f));
	
	vec3 ZeroV3 = { 0.0f, 0.0f, 0.0f };
	//PushLine(LineVertices, &LineVertexCount, ZeroV3, AXY, Vec3(1.0f, 0.0f, 1.0f), IdentityMat4);
	//PushLine(LineVertices, &LineVertexCount, ZeroV3, AXZ, Vec3(1.0f, 1.0f, 0.0f), IdentityMat4);
	//PushLine(LineVertices, &LineVertexCount, ZeroV3, AYZ, Vec3(0.0f, 1.0f, 1.0f), IdentityMat4);
	
	//PushCube(LineVertices, &LineVertexCount, ZeroV3, 1.0f, ZeroV3, Rotation);
	PushCuboid(LineVertices, &LineVertexCount, ZeroV3, Size, Vec3(0.0f, 0.0f, 0.0f), Rotation);
	
	PushLine(LineVertices, &LineVertexCount, ZeroV3, AngularVelocity, Vec3(1.0f, 0.0f, 1.0f), IdentityMat4);
	PushLine(LineVertices, &LineVertexCount, ZeroV3, L, Vec3(0.0f, 0.5f, 1.0f), IdentityMat4);
	
	vec3 TracePoint = Vec3(0.0f, HalfSize.Y, 0.0f);
	//TracePoint = HalfSize;
	TraceAdd(&TopTrace, MultMat4Vec3(Rotation, TracePoint));
	PushTrace(LineVertices, &LineVertexCount, &TopTrace, Vec3(0.3f, 0.5f, 0.8f));
	
	TraceAdd(&OmegaTrace, AngularVelocity);
	PushTrace(LineVertices, &LineVertexCount, &OmegaTrace, Vec3(1.0f, 0.1f, 0.8f));
	
	vec3 NormL = NormalizeVec3(L);
	vec3 OmegaInL = ScaleVec3(NormL, DotVec3(NormL, AngularVelocity));
	vec3 OmegaOrthoL = SubVec3(AngularVelocity, OmegaInL);
	PushLine(LineVertices, &LineVertexCount, ZeroV3, OmegaOrthoL, Vec3(0.1f, 0.5f, 0.2f), IdentityMat4);
	
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	glDrawArrays(GL_LINES, 0, LineVertexCount);
	
#if 0
	vec4 LightDir = {0.0f, -1.0f, 0.0f, 1.0f};
	vec4 ModelColor = {1.0f, 0.0f, 1.0f, 1.0f};

	mat4 ShadowCubes[ShadowCubeCount];
	for(int I = 0; I < ShadowCubeCount; ++I) {
		mat4 Rotation = IdentityMat4;
		Rotation = MultMat4(RotationXMat4(Accu), Rotation);
		Rotation = MultMat4(RotationYMat4(0.4f), Rotation);
		Rotation = MultMat4(RotationZMat4(Accu*0.8f), Rotation);
		vec3 Scale = {1.0f, 1.0f, 1.0f};
		ShadowCubes[I] = InvTransformMat4(Cubes[I].Position, Rotation, Scale);
		Cubes[I].Mat = TransformMat4(Cubes[I].Position, Rotation, Scale);
	}
	
	glUniform4fv(LocLightDir, 1, LightDir.E);
	glUniformMatrix4fv(LocMatProj, 1, GL_TRUE, MatProj.E);
	glUniformMatrix4fv(LocMatView, 1, GL_TRUE, MatView.E);
	glUniformMatrix4fv(LocShadowCubes, ShadowCubeCount, GL_TRUE, (float *)ShadowCubes);
	glUniform1i(LocShadowCubeCount, ShadowCubeCount);
	
	for(int I = 0; I < ShadowCubeCount; ++I) {
		DrawModel(&Cubes[I]);
	}
	DrawModel(&Ground);
#endif
}
