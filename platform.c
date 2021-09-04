#define ArrayCount(X) (sizeof(X) / sizeof(*X))

typedef struct {
	int Width, Height;
	int MouseX, MouseY;
	float DeltaTime;
	int MouseDownLeft;
	int MouseDownMiddle;
	int MouseDownRight;
	float ScrollDelta;
} context;

void Setup();
void Draw(context *Context);

#ifdef __unix__
#include "platform_linux.c"
#define OutputDebugString(X)
#else
#include "platform_win32.c"
#endif

#include "vecmath.c"

typedef struct {
	vec3 Position;
	vec2 TexCoord;
	vec3 Normal;
} vertex;

typedef struct {
	int IndexCount, VertexCount;
	vertex *Vertices;
	int *Indices;
	GLuint VAO;
	GLuint VBO;
	GLuint IBO;
} mesh;

int CompareVertex(vertex A, vertex B) {
	return A.Position.X == B.Position.X
	       && A.Position.Y == B.Position.Y
	       && A.Position.Z == B.Position.Z
	       && A.TexCoord.X == B.TexCoord.X
	       && A.TexCoord.Y == B.TexCoord.Y
	       && A.Normal.X == B.Normal.X
	       && A.Normal.Y == B.Normal.Y
	       && A.Normal.Z == B.Normal.Z;
}

char *ReadEntireFile(const char *Path) {
	FILE *File = fopen(Path, "rb");
	fseek(File, 0, SEEK_END);
	int FileLength = ftell(File);
	fseek(File, 0, SEEK_SET);
	char *Contents = calloc(1, FileLength + 1);
	fread(Contents, FileLength, 1, File);
	return Contents;
}

static float ParseFloatSeekEnd(char *Data, int *Index) {
	// *Index is at first digit
	char *It0 = Data + *Index;
	char *It = It0;
	label_Continue:
	switch(*It) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
		case 'e':
		case '+':
		case '-':
		case 'f':
		++It;
		goto label_Continue;
	}
	int Length = It - It0;
	float Result = 0.0f;
	char Tmp = *It;
	*It = 0;
	if(Length > 0) {
		Result = atof(It0);
	}
	*It = Tmp;
	*Index += Length;
	return Result;
}

static float ParseIntSeekEnd(char *Data, int *Index) {
	// *Index is at first digit
	char *It0 = Data + *Index;
	char *It = It0;
	label_Continue:
	switch(*It) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '+':
		case '-':
		++It;
		goto label_Continue;
	}
	int Length = It - It0;
	float Result = 0.0f;
	char Tmp = *It;
	*It = 0;
	if(Length > 0) {
		Result = atoi(It0);
	}
	*It = Tmp;
	*Index += Length;
	return Result;
}

static mesh LoadOBJ(const char *Path) {
	char *Data = ReadEntireFile(Path);

	// NOTE(blackedout01): Count
	int Index = 0;
	int CountV = 0;
	int CountVT = 0;
	int CountVN = 0;
	int CountF = 0;
	for(;;) {

		if(Data[Index] == 0)
			break;

		// Index is at line start
		if(Data[Index] == 'v') {
			++Index;
			if(Data[Index] == 't') {
				++CountVT;
			} else if(Data[Index] == 'n') {
				++CountVN;
			} else {
				++CountV;
			}
		} else if(Data[Index] == 'f') {
			++CountF;
		}
		for(;; ++Index) {
			if(Data[Index] == 0) goto breakOuterLoop;
			if(Data[Index] == '\n') break;
		}
		++Index;
	}
	breakOuterLoop:
	
	float *AttribV = malloc(3 * CountV * sizeof(float));
	int IndexV = 0;
	float *AttribVT = malloc(2 * CountVT * sizeof(float));
	int IndexVT = 0;
	float *AttribVN = malloc(3 * CountVN * sizeof(float));
	int IndexVN = 0;
	int *FaceIndices = malloc(9 * CountF * sizeof(int));
	int IndexFace = 0;

	Index = 0;
	for(;;) {

		if(Data[Index] == 0)
			break;

		// Index is at line start
		if(Data[Index] == 'v') {
			++Index;
			int ItCount;
			float *Attrib;
			if(Data[Index] == 't') {
				++Index;
				ItCount = 2;
				Attrib = AttribVT + IndexVT;
				IndexVT += ItCount;
			} else if(Data[Index] == 'n') {
				++Index;
				ItCount = 3;
				Attrib = AttribVN + IndexVN;
				IndexVN += ItCount;
			} else {
				ItCount = 3;
				Attrib = AttribV + IndexV;
				IndexV += ItCount;
			}

			for(int J = 0; J < ItCount; ++J) {
				while(Data[Index] == ' ') {
					++Index;
				}
				Attrib[J] = ParseFloatSeekEnd(Data, &Index);
			}
		} else if(Data[Index] == 'f') {
			++Index;
			for(int J = 0; J < 9; ++J) {
				while(Data[Index] == ' ' || Data[Index] == '/') {
					++Index;
					if(Data[Index] == '/') break;
				}
				FaceIndices[IndexFace++] = ParseIntSeekEnd(Data, &Index);
			}
		}
		for(;; ++Index) {
			if(Data[Index] == 0) goto breakOuterLoop;
			if(Data[Index] == '\n') break;
		}
		++Index;
	}

	free(Data);

	mesh Result = {0};
	Result.IndexCount = 3*CountF;
	Result.Indices = malloc(Result.IndexCount*sizeof(int));
	int MaxVertexCount = Result.IndexCount;
	Result.Vertices = malloc(MaxVertexCount*sizeof(vertex));
	
	for(int FaceIndex = 0; FaceIndex < Result.IndexCount; ++FaceIndex) {
		IndexV = FaceIndices[3*FaceIndex + 0] - 1;
		IndexVT = FaceIndices[3*FaceIndex + 1] - 1;
		IndexVN = FaceIndices[3*FaceIndex + 2] - 1;
		vertex Vertex;
		Vertex.Position = ((vec3 *)AttribV)[IndexV];
		Vertex.TexCoord = ((vec2 *)AttribVT)[IndexVT];
		Vertex.Normal = ((vec3 *)AttribVN)[IndexVN];
		for(int I = Result.VertexCount - 1; I >= 0; --I) {
			if(CompareVertex(Result.Vertices[I], Vertex)) {
				Result.Indices[FaceIndex] = I;
				goto label_ContinueOuter;
			}
		}
		Result.Indices[FaceIndex] = Result.VertexCount;
		Result.Vertices[Result.VertexCount++] = Vertex;

		label_ContinueOuter:;
	}

	Result.Vertices = realloc(Result.Vertices, Result.VertexCount*sizeof(vertex));

	free(AttribV);
	free(AttribVT);
	free(AttribVN);
	free(FaceIndices);

	return Result;
}

GLuint CompileShader(const char *VertexPath, const char *FragmentPath) {
	GLint Success0;

	// NOTE: Compile vertex shader
	char *VertexShader = ReadEntireFile(VertexPath);
	GLuint VID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VID, 1, (const GLchar * const*)&VertexShader, 0);
	glCompileShader(VID);
	free(VertexShader);
	
	glGetShaderiv(VID, GL_COMPILE_STATUS, &Success0);
	if (Success0 == GL_FALSE) {
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
	glShaderSource(FID, 1, (const GLchar * const*)&FragShader, 0);
	glCompileShader(FID);
	free(FragShader);
	
	glGetShaderiv(FID, GL_COMPILE_STATUS, &Success0);
	if (Success0 == GL_FALSE) {
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

	glGetProgramiv(Program, GL_LINK_STATUS, &Success0);
	if (Success0 == GL_FALSE) {
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