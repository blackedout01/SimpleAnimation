#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glx.h>

static uint64_t MicroTime() {
	struct timeval Time;
	struct timezone TimeZone;
	gettimeofday(&Time, &TimeZone);
	return Time.tv_sec*1000000 + Time.tv_usec;
}

int main() {
	context Context = {0};
	Display *Displ;
	Window Win;
	GLXContext RenderContext;
	int Running;
	XVisualInfo *VisualInfo;
	XEvent Event;
	int Screen;
	Running = 1;
	GLint Attribs[] = {
		GLX_RGBA,
		GLX_DEPTH_SIZE,
		24,
		GLX_DOUBLEBUFFER,
		None
	};
	Colormap Cmap;
	
	XSetWindowAttributes SetWinAttribs;
	XWindowAttributes WinAttribs;
	Displ = XOpenDisplay(0);
	if(Displ == 0) {
		return 1;
	}
	Screen = DefaultScreen(Displ);
	VisualInfo = glXChooseVisual(Displ, 0, Attribs);
	Cmap = XCreateColormap(Displ, RootWindow(Displ, Screen), VisualInfo->visual, AllocNone);
	SetWinAttribs.colormap = Cmap;
	SetWinAttribs.event_mask = ExposureMask | KeyPressMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask;
	Win = XCreateWindow(Displ, RootWindow(Displ, Screen), 0, 0, 1440, 900,
		0, VisualInfo->depth, InputOutput, VisualInfo->visual,
		CWColormap | CWEventMask, &SetWinAttribs);
	XMapWindow(Displ, Win);

	RenderContext = glXCreateContext(Displ, VisualInfo, 0, GL_TRUE);
	glXMakeCurrent(Displ, Win, RenderContext);

	XGetWindowAttributes(Displ, Win, &WinAttribs);
	Context.Width = WinAttribs.width;
	Context.Height = WinAttribs.height;
	Setup();
	
	uint64_t LastMicroTime = MicroTime();
	uint64_t FirstMicroTime = MicroTime();
	int Its = 0;
	while(Running) {
		while(XPending(Displ)) {
			XNextEvent(Displ, &Event);

			// NOTE(blackedout01): See https://tronche.com/gui/x/xlib/events/types.html
			switch(Event.type) {
				case Expose:
				XGetWindowAttributes(Displ, Win, &WinAttribs);
				if(Context.Width != WinAttribs.width || Context.Height != WinAttribs.height) {
					glViewport(0, 0, WinAttribs.width, WinAttribs.height);
				}
				Context.Width = WinAttribs.width;
				Context.Height = WinAttribs.height;
				break;
				case MotionNotify:
				Context.MouseX = Event.xmotion.x;
				Context.MouseY = Event.xmotion.y;
				break;
				case ButtonPress:
				switch(Event.xbutton.button) {
					case Button1:
					Context.MouseDownLeft = 1;
					break;
					case Button2:
					Context.MouseDownMiddle = 1;
					break;
					case Button3:
					Context.MouseDownRight = 1;
					break;
					case Button4:
					Context.ScrollDelta -= 1.0f;
					break;
					case Button5:
					Context.ScrollDelta += 1.0f;
					break;
				}
				break;
				case ButtonRelease:
				switch(Event.xbutton.button) {
					case Button1:
					Context.MouseDownLeft = 0;
					break;
					case Button2:
					Context.MouseDownMiddle = 0;
					break;
					case Button3:
					Context.MouseDownRight = 0;
					break;
				}
				break;
				case KeyPress:
				break;
			}
		}
		Its++;
		uint64_t NewMicroTime = MicroTime();
		Context.DeltaTime = (NewMicroTime - LastMicroTime)/1000000.0f;
		// Print time per frame:
		// printf("%f\n", (NewMicroTime - FirstMicroTime)/1000000.0f/Its);
		LastMicroTime = NewMicroTime;

		Draw(&Context);

		Context.ScrollDelta = 0.0f;

		glXSwapBuffers(Displ, Win);
	}

	glXMakeCurrent(Displ, None, 0);
	glXDestroyContext(Displ, RenderContext);
	XDestroyWindow(Displ, Win);
	XCloseDisplay(Displ);
	return 0;
}