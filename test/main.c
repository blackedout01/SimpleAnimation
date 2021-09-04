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

void Setup() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_FRAMEBUFFER_SRGB);

	ShaderProgram = CompileShader("vertex.glsl", "fragment.glsl");

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
}

static float Accu = 0.0f;

// 0 to 2pi
static float H = 0.0f;

// -pi/2 to pi/2
static float V = 0.0f;

static int LastMouseX = 0;
static int LastMouseY = 0;

static float Zoom = 30;

void Draw(context *Context) {
	float Aspect = Context->Width/(float)Context->Height;
	mat4 MatProj = ProjectionMatrix(1.0f, Aspect, 0.1f, 1000.0f);
	Zoom *= 1 + Context->ScrollDelta/10;
	vec3 T = {0, 0, -Zoom};
	

	float MouseDX = (float)(Context->MouseX - LastMouseX);
	float MouseDY = (float)(Context->MouseY - LastMouseY);
	LastMouseX = Context->MouseX;
	LastMouseY = Context->MouseY;

	if(Context->MouseDownLeft) {
		H += MouseDX*0.01f;
		V += MouseDY*0.01f;
	}
	mat4 MatView = IdentityMat4;
	MatView = MultMat4(RotationYMat4(H), MatView);
	MatView = MultMat4(RotationXMat4(V), MatView);
	MatView = MultMat4(TranslationMat4(T), MatView);

	vec4 LightDir = {0.0f, -1.0f, 0.0f, 1.0f};
	vec4 ModelColor = {1.0f, 0.0f, 1.0f, 1.0f};

	Accu += 0.1 * Context->DeltaTime;
	glClearColor(0.81f, 0.98f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	glUseProgram(ShaderProgram);
	glUniform4fv(LocLightDir, 1, LightDir.E);
	glUniformMatrix4fv(LocMatProj, 1, GL_TRUE, MatProj.E);
	glUniformMatrix4fv(LocMatView, 1, GL_TRUE, MatView.E);
	glUniformMatrix4fv(LocShadowCubes, ShadowCubeCount, GL_TRUE, (float *)ShadowCubes);
	glUniform1i(LocShadowCubeCount, ShadowCubeCount);
	
	for(int I = 0; I < ShadowCubeCount; ++I) {
		DrawModel(&Cubes[I]);
	}
	DrawModel(&Ground);
}
