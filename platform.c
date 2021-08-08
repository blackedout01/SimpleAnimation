#define ArrayCount(X) (sizeof(X) / sizeof(*X))

#include <stdio.h>

typedef struct {
	int Width, Height;
} context;

void Setup();
void Draw(context *Context);

#ifdef __unix__
#include "platform_linux.c"
#define OutputDebugString(X)
#else
#include "platform_win32.c"
#endif

char *ReadEntireFile(const char *Path) {
	FILE *File = fopen(Path, "rb");
	fseek(File, 0, SEEK_END);
	int FileLength = ftell(File);
	fseek(File, 0, SEEK_SET);
	char *Contents = calloc(1, FileLength + 1);
	fread(Contents, FileLength, 1, File);
	return Contents;
}

GLuint CompileShader(const char *VertexPath, const char *FragmentPath) {
	GLint Success;

	// NOTE: Compile vertex shader
	char *VertexShader = ReadEntireFile(VertexPath);
	GLuint VID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VID, 1, &VertexShader, 0);
	glCompileShader(VID);
	free(VertexShader);
	
	glGetShaderiv(VID, GL_COMPILE_STATUS, &Success);
	if (Success == GL_FALSE) {
		GLchar Buf[1024] = {0};
		GLsizei Length = 0;
		glGetShaderInfoLog(VID, ArrayCount(Buf), &Length, Buf);
		printf("%.*s", Length, Buf);
		OutputDebugString(Buf);
		return 1;
	}

	// NOTE: Compile fragement shader
	char *FragShader = ReadEntireFile(FragmentPath);
	GLuint FID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FID, 1, &FragShader, 0);
	glCompileShader(FID);
	free(FragShader);
	
	glGetShaderiv(FID, GL_COMPILE_STATUS, &Success);
	if (Success == GL_FALSE) {
		GLchar Buf[1024] = {0};
		GLsizei Length = 0;
		glGetShaderInfoLog(FID, ArrayCount(Buf), &Length, Buf);
		printf("%.*s", Length, Buf);
		OutputDebugString(Buf);
		return 1;
	}

	// NOTE: Create shader program
	GLuint Program = glCreateProgram();
	glAttachShader(Program, VID);
	glAttachShader(Program, FID);
	glLinkProgram(Program);

	glGetProgramiv(Program, GL_LINK_STATUS, &Success);
	if (Success == GL_FALSE) {
		GLchar Buf[1024] = {0};
		GLsizei Length = 0;
		glGetProgramInfoLog(Program, ArrayCount(Buf), &Length, Buf);
		printf("%.*s", Length, Buf);
		OutputDebugString(Buf);
		return 1;
	}

	glDetachShader(Program, VID);
	glDetachShader(Program, FID);
	glDeleteShader(VID);
	glDeleteShader(FID);

	return Program;	
}