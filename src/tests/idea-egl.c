/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Design directions' tests
 *     toolset: ISO C '99 + OpenGL 3.3 + EGL + Xlib
 *     content: EGL context creation test (text/x-c)
 *     version: 1.0 OS
 */

#include "output.h"
#include <stdlib.h>
#include <gtk/gtk.h>
#include <GL/gl.h>
#include <EGL/egl.h>

EGLint major, minor;

int main(int argc, char* argv[]) {
	hvLogInit(HV_LOG_INFO, stdout);
	hvPrint("HyperVision design approach test programme: OpenGL + EGL");

	/* Create a Window */
	gtk_init(&argc, &argv);
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect (window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show (window);

	/* Initialise display */	
	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display, &major, &minor);
	hvErrorCheckEGL("Unable to initialise display");
	hvInfo2("Detected EGL version: ", eglQueryString(display, EGL_VERSION));

	/* Choose visual and initalise window */
	EGLint configs;
	EGLConfig config;
	const EGLint attribs[] = {
		EGL_CONFIG_CAVEAT, EGL_NONE,
		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
		EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};
	eglChooseConfig(display, (const EGLint *) &attribs, &config, 1, &configs);
	hvErrorCheckEGL("Unable to retrieve visual");

	EGLSurface surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType) window, NULL);
	hvErrorCheckEGL("Unable to create EGL surface");

	eglBindAPI(EGL_OPENGL_API);
	hvErrorCheckEGL("Unable to bing OpenGL API");

	EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
	hvErrorCheckEGL("Unable to create context");
	if (context != EGL_NO_CONTEXT)
		hvInfo("Hurra!");
	eglMakeCurrent(display, surface, surface, context);
	hvErrorCheckEGL("Unable to activate rendering context");

	/* OpenGL API */
	eglBindAPI(EGL_OPENGL_API);
	hvErrorCheckEGL("Unable to bind OpenGL API");

	glGetError();
	glClearColor(0.0f, 0.5f, 0.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	hvErrorCheckGL("Could not set OpenGL culling options");
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	
	eglSwapBuffers(display, surface);
	
	gtk_main ();
	eglDestroyContext(display, context);
	eglTerminate(display);
	exit(EXIT_SUCCESS);
}

