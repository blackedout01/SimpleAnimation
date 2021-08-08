#include "platform.c"

static GLuint ShaderProgram;
static GLuint VAO;
static GLuint VBO;

void Setup() {
	ShaderProgram = CompileShader("vertex.glsl", "fragment.glsl");
	
	float Data[] = {
		0, 0,
		0, -1,
		1, 0,
		1, -1
	};	

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Data), Data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
	glEnableVertexAttribArray(0);
}

void Draw(context *Context) {
	glViewport(0, 0, Context->Width, Context->Height);
	
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(VAO);

	glUseProgram(ShaderProgram);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}