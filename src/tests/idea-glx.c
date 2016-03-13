/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Design directions' tests
 *     toolset: ISO C '99 + OpenGL 3.3 + GLX + Xlib
 *     content: GLX context creation test (text/x-c)
 *     version: 1.0 OS
 */

#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

/* Definitions for GLX extensions beyond version 1.4 */
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
 
static Bool hvInSet(const char *ext_lst, const char *ext) {
	const char *start, *where, *term;
	if (strchr(ext, ' ') || *ext == '\0')
		return False;
	for (start = ext_lst; ; ) {
		where = strstr(start, ext);
		if (!where) break;
		term = where + strlen(ext);
		if (where == start || *(where - 1) == ' ')
			if (*term == ' ' || *term == '\0')
				return True;
		start = term;
	}
	return False;
}
 
int main() {

	hvLogInit(HV_LOG_INFO, stdout);
	hvPrint("HyperVision design approach test programme: X + GLX + OpenGL");

	/* Open X Display */
	Display *x_dsp = XOpenDisplay(NULL);
	hvErrorCheck(!x_dsp, "Failed to open X display");

	/* Check and report GLX version*/ 
	int glx_maj = 0, glx_min = 0;
	glXQueryVersion(x_dsp, &glx_maj, &glx_min);
	hvErrorCheck(glx_maj * 10 + glx_min < 14, "Invalid GLX version (1.4 required)"); //*1
	hvInfo("GLX version: ", glXGetClientString(x_dsp, GLX_VERSION));

	/* Choose best GLX FB configuration and get the GLX visual */
	static int glx_fb_atr[] = {
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_CONFIG_CAVEAT   , GLX_NONE,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		None
    };
	int glx_fb_cnt;
	GLXFBConfig *glx_fb_cfg = glXChooseFBConfig(x_dsp, DefaultScreen(x_dsp), glx_fb_atr, &glx_fb_cnt);
	hvErrorCheck(!glx_fb_cfg, "Failed to retrieve a framebuffer config");
	// Pick the FB config with the most samples per pixel
	int top_fbc = 0, top_smp = 0, smp;
	for (int i = 0; i < glx_fb_cnt; i++) {
		glXGetFBConfigAttrib(x_dsp, glx_fb_cfg[i], GLX_SAMPLES, &smp);
		if (smp > top_smp) top_fbc = i, top_smp = smp;
	}
	GLXFBConfig glx_fbc = glx_fb_cfg[top_fbc];
	XFree(glx_fb_cfg);
	XVisualInfo *glx_vsl = glXGetVisualFromFBConfig(x_dsp, glx_fbc);
	hvInfo("Chosen visual ID = ", hvHex(glx_vsl->visualid));

	/* Create and map X window */
	Colormap x_cmap = XCreateColormap(x_dsp, RootWindow(x_dsp, glx_vsl->screen), glx_vsl->visual, AllocNone);
	XSetWindowAttributes x_swa;
	x_swa.colormap = x_cmap;
	x_swa.background_pixmap = None;
	x_swa.border_pixel = 0;
	x_swa.event_mask = StructureNotifyMask;
	Window x_wnd = XCreateWindow(x_dsp, RootWindow(x_dsp, glx_vsl->screen), 0, 0, 100, 100, 0, glx_vsl->depth,
	                             InputOutput, glx_vsl->visual, CWBorderPixel|CWColormap|CWEventMask, &x_swa);
	hvErrorCheck(!x_wnd, "Failed to create window");
	XFree(glx_vsl);
	XStoreName(x_dsp, x_wnd, "HyperVision GLX test");
	XMapWindow(x_dsp, x_wnd);

	/* Retreive GLX_ARB_create_context extension and create OpenGL 3.0 context */
	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};
	const char *glx_exts = glXQueryExtensionsString(x_dsp, DefaultScreen(x_dsp));
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddress((const GLubyte*) "glXCreateContextAttribsARB");
	hvErrorCheck(!hvInSet(glx_exts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB, "GLX_ARB_create_context extension missing");
	GLXContext glx_ctx = glXCreateContextAttribsARB(x_dsp, glx_fbc, 0, True, context_attribs );
 
	if (!glXIsDirect(x_dsp, glx_ctx)) {
		hvInfo("GLX rendering context: indirect");
	}
	else	{
		hvInfo("GLX rendering context: DRI");
	}

	/* Render OpenGL scene */
	glXMakeCurrent(x_dsp, x_wnd, glx_ctx);

	glClearColor(0, 0.5, 1, 1);
	glClear (GL_COLOR_BUFFER_BIT);
	glXSwapBuffers(x_dsp, x_wnd);
	sleep(1);

	glClearColor(1, 0.5, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glXSwapBuffers(x_dsp, x_wnd);
	sleep(1);

	glXMakeCurrent(x_dsp, 0, 0);

	/* Release used resources */
	glXDestroyContext(x_dsp, glx_ctx);
	XDestroyWindow(x_dsp, x_wnd);
	XFreeColormap(x_dsp, x_cmap);
	XCloseDisplay(x_dsp);
}

/*
1 The GLX vesrion 1.4 is required because of the use of:
	glXGetProcAddress function,
	multisampling capabilities of GLXFBConfig.

Function vocabulary:
	Display* XOpenDisplay(char* display_name)
		display_name := protocol/hostname:display.screen
		NULL = use DISPLAY environment variable as display_name
	XCloseDisplay(Display*)
		closes the display
	int XDefaultScreen(Display) or macro DefaultScreen(Display)
		return screen as set by display_name string (e.g. DISPLAY EV)
	
*/

