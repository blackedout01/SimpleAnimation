#include "../platform.c"

static GLuint ShaderProgram;
static GLuint VAO;
static GLuint VBO;
static GLuint IBO;
static GLint LocMatProj;
static GLint LocMatView;
static GLint LocMatModel;
static GLint LocLightDir;
static GLint LocModelColor;

static model Model;

void Setup() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_FRAMEBUFFER_SRGB);

	ShaderProgram = CompileShader("vertex.glsl", "fragment.glsl");

	Model = LoadOBJ("cube.obj");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ARRAY_BUFFER, Model.VertexCount*sizeof(vertex), Model.Vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Model.IndexCount*sizeof(int), Model.Indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(3*sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(5*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	LocMatProj = glGetUniformLocation(ShaderProgram, "MatProj");
	LocMatView = glGetUniformLocation(ShaderProgram, "MatView");
	LocMatModel = glGetUniformLocation(ShaderProgram, "MatModel");
	LocLightDir = glGetUniformLocation(ShaderProgram, "LightDir");
	LocModelColor = glGetUniformLocation(ShaderProgram, "ModelColor"); 
}

static float Accu = 0.0f;

// 0 to 2pi
static float H = 0.0f;

// -pi/2 to pi/2
static float V = 0.0f;

static int LastMouseX = 0;
static int LastMouseY = 0;

void Draw(context *Context) {
	float Aspect = Context->Width/(float)Context->Height;
	mat4 MatProj = ProjectionMatrix(1.0f, Aspect, 0.1f, 1000.0f);
	vec4 T = {0, 0, -3.0f, 0};
	mat4 MatModel = MultMat4(RotationXMat4(0.4f), RotationYMat4(Accu));

	float MouseDX = (float)(Context->MouseX - LastMouseX);
	float MouseDY = (float)(Context->MouseY - LastMouseY);
	LastMouseX = Context->MouseX;
	LastMouseY = Context->MouseY;

	if(Context->MouseDown) {
		H += MouseDX*0.01f;
		V += MouseDY*0.01f;
		glDisable(GL_MULTISAMPLE);
	}
	else {
		glEnable(GL_MULTISAMPLE);
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

	glUseProgram(ShaderProgram);
	glUniformMatrix4fv(LocMatProj, 1, GL_TRUE, MatProj.E);
	glUniformMatrix4fv(LocMatView, 1, GL_TRUE, MatView.E);
	glUniformMatrix4fv(LocMatModel, 1, GL_TRUE, MatModel.E);
	glUniform4fv(LocLightDir, 1, LightDir.E);
	glUniform4fv(LocModelColor, 1, ModelColor.E);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Model.IndexCount, GL_UNSIGNED_INT, 0);
}
