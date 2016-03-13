/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Core modules
 *     toolset: ISO C '99 + OpenGL 3.3 + GLUT 2.6
 *     content: HV::OpenGL module header (text/x-h)
 *     version: 1.0 OS
 *
 * This module preserves a uniform discovery of dependence on OpenGL.
 * Include it instead of OpenGL headres; and specify in Makefile's XDEPEND list for
 * automatic discovery and proper linking in case of a conditional dependence on OpenGL.
 */

#ifndef HV_DS1_OPENGL_H
#define HV_DS1_OPENGL_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "output.h"

enum event_e {
	HV_RESHAPE, HV_DISPLAY, HV_KEYBOARD, HV_SPECIAL, HV_IDLE
};

void hvAppInitWindow(int width, int height, char* title);
void hvAppInitCanvas();
void hvAppEvent(enum event_e event, void (*function)());

#endif /* defined HV_DS1_OPENGL_H */

