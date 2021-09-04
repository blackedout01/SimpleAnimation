#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")

static PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
static PFNGLCREATESHADERPROC glCreateShader;
static PFNGLSHADERSOURCEPROC glShaderSource;
static PFNGLCOMPILESHADERPROC glCompileShader;
static PFNGLGETSHADERIVPROC glGetShaderiv;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
static PFNGLCREATEPROGRAMPROC glCreateProgram;
static PFNGLATTACHSHADERPROC glAttachShader;
static PFNGLLINKPROGRAMPROC glLinkProgram;
static PFNGLGETPROGRAMIVPROC glGetProgramiv;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
static PFNGLDETACHSHADERPROC glDetachShader;
static PFNGLDELETESHADERPROC glDeleteShader;
static PFNGLUSEPROGRAMPROC glUseProgram;
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLBUFFERDATAPROC glBufferData;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
static PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
static PFNGLUNIFORM4FVPROC glUniform4fv;
static PFNGLUNIFORM1IPROC glUniform1i;

static BOOL Running = TRUE;
static context Context;

LRESULT Wndproc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	switch(Message) {
		case WM_CLOSE:
		PostQuitMessage(0);
		Running = FALSE;
		break;
		case WM_SIZE:
		Context.Width = LOWORD(LParam);
		Context.Height = HIWORD(LParam);
		break;
		case WM_MOUSEMOVE:
		Context.MouseX = GET_X_LPARAM(LParam);
		Context.MouseY = GET_Y_LPARAM(LParam);
		break;
		case WM_LBUTTONDOWN:
		Context.MouseDown = 1;
		break;
		case WM_LBUTTONUP:
		Context.MouseDown = 0;
		break;
		default:
		break;
	}
	
	return DefWindowProcA(Window, Message, WParam, LParam);
}

/*
typedef void (APIENTRY *DEBUGPROC)(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam);
*/

static void DebugCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar *Message, const void *UserParam) {
	OutputDebugString(Message);
	OutputDebugString("\n");
}


INT WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, INT ShowCmd) {
	
	char ClassName[] = "TestClass";
	WNDCLASSA WindowClass = {
		CS_OWNDC,	// UINT      style;
		Wndproc,	// WNDPROC   lpfnWndProc;
		0,			// int       cbClsExtra;
		0,			// int       cbWndExtra;
		Instance,	// HINSTANCE hInstance;
		0,			// HICON     hIcon;
		0,			// HCURSOR   hCursor;
		0,			// HBRUSH    hbrBackground;
		0,			// LPCSTR    lpszMenuName;
		ClassName	// LPCSTR    lpszClassName;
	};
	
	RegisterClassA(&WindowClass);
	
	//DWORD WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	DWORD WindowStyle = WS_OVERLAPPEDWINDOW;
	HWND Window = CreateWindowA(
		ClassName,		// LPCTSTR lpClassName
		"Hello",		// LPCTSTR lpWindowName
		WindowStyle,	// DWORD dwStyle
		CW_USEDEFAULT,	// int x
		CW_USEDEFAULT,	// int y
		CW_USEDEFAULT,	// int nWidth
		CW_USEDEFAULT,	// int nHeight
		0,				// HWND hWndParent
		0,				// HMENU hMenu
		Instance,		// HINSTANCE hInstance
		0 				// LPVOID lpParam
	);
	
	HDC DeviceContext = GetDC(Window);
	
	DWORD FormatFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	PIXELFORMATDESCRIPTOR PixelFormat = {
		sizeof(PIXELFORMATDESCRIPTOR),	// WORD  nSize;
		1,								// WORD  nVersion;
		FormatFlags,					// DWORD dwFlags;
		PFD_TYPE_RGBA,					// BYTE  iPixelType;
		32,								// BYTE  cColorBits;
		0,								// BYTE  cRedBits;
		0,								// BYTE  cRedShift;
		0,								// BYTE  cGreenBits;
		0,								// BYTE  cGreenShift;
		0,								// BYTE  cBlueBits;
		0,								// BYTE  cBlueShift;
		0,								// BYTE  cAlphaBits;
		0,								// BYTE  cAlphaShift;
		0,								// BYTE  cAccumBits;
		0,								// BYTE  cAccumRedBits;
		0,								// BYTE  cAccumGreenBits;
		0,								// BYTE  cAccumBlueBits;
		0,								// BYTE  cAccumAlphaBits;
		24,								// BYTE  cDepthBits;
		8,								// BYTE  cStencilBits;
		0,								// BYTE  cAuxBuffers;
		PFD_MAIN_PLANE,					// BYTE  iLayerType;
		0,								// BYTE  bReserved;
		0,								// DWORD dwLayerMask;
		0,								// DWORD dwVisibleMask;
		0								// DWORD dwDamageMask;
	};
	
	// TODO: Enumerate available pixel formats?
	INT FormatIndex = ChoosePixelFormat(DeviceContext, &PixelFormat);
	BOOL Result = SetPixelFormat(DeviceContext, FormatIndex, &PixelFormat);
	HGLRC RenderContext0 = wglCreateContext(DeviceContext);
	wglMakeCurrent(DeviceContext, RenderContext0);

	char Buf2[128] = {0};
	sprintf(Buf2, "FormatIndex: %d\n", FormatIndex);
	OutputDebugString(Buf2);
	
	// https://www.opengl.org/archives/resources/features/OGLextensions/
	// TODO: Check for NULL Result
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");

	
	wglMakeCurrent(0, 0);
	wglDeleteContext(RenderContext0);
	ReleaseDC(Window, DeviceContext);
	DestroyWindow(Window);

	Window = CreateWindowA(
		ClassName,		// LPCTSTR lpClassName
		"Hello",		// LPCTSTR lpWindowName
		WindowStyle | WS_VISIBLE,	// DWORD dwStyle
		CW_USEDEFAULT,	// int x
		CW_USEDEFAULT,	// int y
		CW_USEDEFAULT,	// int nWidth
		CW_USEDEFAULT,	// int nHeight
		0,				// HWND hWndParent
		0,				// HMENU hMenu
		Instance,		// HINSTANCE hInstance
		0 				// LPVOID lpParam
	);
	DeviceContext = GetDC(Window);

	UINT numFormats;
	int FormatAttribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 8,
		//WGL_COLORSPACE_EXT, WGL_COLORSPACE_SRGB_EXT,
		0
	};
	Result = wglChoosePixelFormatARB(DeviceContext, FormatAttribs, 0, 1, &FormatIndex, &numFormats);
	char Buf[128] = {0};
	sprintf(Buf, "FormatIndex: %d\n", FormatIndex);
	OutputDebugString(Buf);
	if(Result == FALSE) {
		OutputDebugString("wglChoosePixelFormatARB: FALSE\n"); 
	}
	PIXELFORMATDESCRIPTOR PixelFormat2 = {
		sizeof(PIXELFORMATDESCRIPTOR),	// WORD  nSize;
		1,								// WORD  nVersion;
		FormatFlags,					// DWORD dwFlags;
		PFD_TYPE_RGBA,					// BYTE  iPixelType;
		32,								// BYTE  cColorBits;
		0,								// BYTE  cRedBits;
		0,								// BYTE  cRedShift;
		0,								// BYTE  cGreenBits;
		0,								// BYTE  cGreenShift;
		0,								// BYTE  cBlueBits;
		0,								// BYTE  cBlueShift;
		0,								// BYTE  cAlphaBits;
		0,								// BYTE  cAlphaShift;
		0,								// BYTE  cAccumBits;
		0,								// BYTE  cAccumRedBits;
		0,								// BYTE  cAccumGreenBits;
		0,								// BYTE  cAccumBlueBits;
		0,								// BYTE  cAccumAlphaBits;
		24,								// BYTE  cDepthBits;
		8,								// BYTE  cStencilBits;
		0,								// BYTE  cAuxBuffers;
		PFD_MAIN_PLANE,					// BYTE  iLayerType;
		0,								// BYTE  bReserved;
		0,								// DWORD dwLayerMask;
		0,								// DWORD dwVisibleMask;
		0								// DWORD dwDamageMask;
	};
	Result = SetPixelFormat(DeviceContext, FormatIndex, &PixelFormat2);
	if(Result == FALSE) {
		OutputDebugString("SetPixelFormat: FALSE\n"); 
	}

	int ContextAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0
	};
	HGLRC RenderContext = wglCreateContextAttribsARB(DeviceContext, 0, ContextAttribs);
	
	BOOL Result1 = wglMakeCurrent(DeviceContext, RenderContext);
	
	OutputDebugString(wglGetExtensionsStringARB(DeviceContext));

	if(Result1 == TRUE)
		OutputDebugString(glGetString(GL_VERSION));
	else
		OutputDebugString("Noooo :(");
	
	wglSwapIntervalEXT(1);
	
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	LARGE_INTEGER Freq;
	LARGE_INTEGER LastTime;
	LARGE_INTEGER Time;
	// NOTE(blackedout01): Windows versions prior to XP not allowed.
	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&LastTime);

	Setup();
	MSG Message;
	while(Running) {
		while(PeekMessageA(&Message, Window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		glViewport(0, 0, Context.Width, Context.Height);

		QueryPerformanceCounter(&Time);
		Context.DeltaTime = (Time.QuadPart - LastTime.QuadPart)/(float)Freq.QuadPart;
		LastTime = Time;

		//char Buf[128] = {0};
		//sprintf(Buf, "DeltaTime: %f\n", Context.DeltaTime);
		//OutputDebugString(Buf);
		Draw(&Context);
		
		#if 0
		GLenum Error = glGetError();
		if(Error != GL_NO_ERROR)
			OutputDebugString("Error");
		#endif

		SwapBuffers(DeviceContext);
	}
	
	return 0;
}