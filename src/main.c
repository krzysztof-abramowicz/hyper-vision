/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *     toolset: ISO C '99 + OpenGL 3.3 + GLUT 2.6
 *     content: HyperVision DS1 demonstration program (text/x-c)
 *     version: 1.2 OS
 */

#include "common.h"
#include "opengl.h"
#include "output.h"
#include "matrix.h"
#include "shader.h"
#include "model.h"

#define PROGRAM_NAME "HyperVision DS1"

GLuint
	frameCount = 0,
	curWidth = 600,
	curHeight = 600;

mat4_t
	testMatrix,
	viewMatrix,
	projectionMatrix;

prog_t
	shaderProgram;

model_t letterH, letterV, axis, cube;

float rotation = 0, camX = 0, camZ = 2;

void hvaReshape(int, int);
void hvaDisplay(void);
void hvaTimer(int);
void hvaIdle(void);
void hvaClean(void);
void hvaSpecial(int, int, int);
void hvaKeyboard(int, int, int);

int main(int argc, char *argv[]) {
	/* Start logging */
	hvLogInit(HV_LOG_DEBUG, stdout);
	hvPrint("Krzysztof Abramowicz (http://abramowicz.info/, k.j.abramowicz@gmail.com)");
	hvPrint("HyperVision Design Stage One (ISO C '99 + OpenGL 3.3 + GLUT 2.6)");
	hvLogTabWidth(4);
/*d
	hvLogLimits();
	hvLogFloats();
*/
	/* Initialise window and canvas */
	hvAppInitWindow(curWidth, curHeight, PROGRAM_NAME);
	hvAppInitCanvas();

	/* Register callbacks */
	hvAppEvent(HV_RESHAPE, hvaReshape);
	hvAppEvent(HV_DISPLAY, hvaDisplay);
	hvAppEvent(HV_SPECIAL, hvaSpecial);
	hvAppEvent(HV_KEYBOARD, hvaKeyboard);
	hvAppEvent(HV_IDLE, hvaIdle);

	/* Build shaders */
	shaderProgram = hvCreateProgram(HV_VSHADER_PHONG, HV_FSHADER_PHONG);
	hvInfo("Shader program built (phong lighting model)");
/*d
	hvProgramInfo(&shaderProgram);
*/
	/* Construct models */
	letterH = hvCreateStockModel(HV_MODEL_H);
	hvModelColor(&letterH, rgb(.1f, .8f, .1f));
	hvInfo("model \"", hvModelName(&letterH), "\" created (", hvNum(hvModelFaces(&letterH)), " triangles)");

	letterV = hvCreateStockModel(HV_MODEL_V);
	hvModelColor(&letterV, rgb(.1f, .1f, .8f));
	hvInfo("model \"", hvModelName(&letterV), "\" created (", hvNum(hvModelFaces(&letterV)), " triangles)");
/*e
	cube = hvCreateStockModel(HV_MODEL_CUBE);
	hvModelColor(&cube, rgb(1.0f, .5f, .0f));
*/
	/* Calculate the view matrix */
	hvMatrixViewCamera(&viewMatrix, vec(camX, 0, camZ), vec(0, 0, 0), vec(0, 1, 0));
	hvDebug("View matrix preview:");
	hvMatrixPrintFC(&viewMatrix);

		/* Playing ground :-) */
		glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPointSize(4.0f);
		if (argc > 0 && argv[0][0] != '\0')
			hvDebug("Prgram name: ", argv[0]);

	/* enter the main loop */
	glutCloseFunc(hvaClean);
	glutTimerFunc(0, hvaTimer, 0);
	hvInfo("Entering main rendering loop...");
	glutMainLoop();	
	hvInfo("Exited successfully");
	hvLogTerm();
}

void hvaReshape(int width, int height) {
	/* Update local state and resize the rendering viewport */
	curWidth = width;
	curHeight = height;
	glViewport(0, 0, curWidth, curHeight);
	hvaKeyboard('p', 0, 0);
}

void hvaDisplay(void)
{
	++frameCount;
	rotation += (rotation < 360.0f) ? 1.0f : 1.0f - 360.0f;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	hvMatrixIdentity(&letterH.matrix);
	hvMatrixTranslate(&letterH.matrix, vec(.0f, .0f, .15f));
	hvMatrixRotateX(&letterH.matrix, rotation);
	hvModelDraw(&letterH, &shaderProgram, &viewMatrix);

	hvMatrixIdentity(&letterV.matrix);
	hvMatrixTranslate(&letterV.matrix, vec(.0f, .0f, -.15f));
	hvMatrixRotateY(&letterV.matrix, rotation);
	hvModelDraw(&letterV, &shaderProgram, &viewMatrix);
/*
	hvMatrixIdentity(&cube.matrix);
	hvMatrixRotateX(&cube.matrix, rotation);
	hvMatrixRotateY(&cube.matrix, rotation);
	hvModelDraw(&cube, &shaderProgram, &viewMatrix);
*/
	glutSwapBuffers();
	glutPostRedisplay();
}

void hvaTimer(int value) {
	if (value != 0) {
		char* TempString = (char*) malloc(512 + strlen(PROGRAM_NAME));
		sprintf(TempString, "%s: %d fps @ %d x %d", PROGRAM_NAME, frameCount * 4, curWidth, curHeight);
		glutSetWindowTitle(TempString);
		free(TempString);
	}
	frameCount = 0;
	glutTimerFunc(250, hvaTimer, 1);
}

void hvaIdle() {
	glutPostRedisplay();
}

void hvaKeyboard(int key, int x, int y) {
	x = y; y = x;
	switch (key) {
		case 'c':
			hvMatrixProjectionCubic(&projectionMatrix, 2.0f);
			hvInfo("Selected projection: CUBIC");
			break;
		case 'o':
			hvMatrixProjectionOrthographic(&projectionMatrix, -0.6f, 0.6f, -1.0f, 1.0f, -2.0f, 2.1f);
			hvInfo("Selected projection: ORTHOGRAPHIC");
			break;
		case 'p':
			hvMatrixProjectionPerspective(&projectionMatrix, 60, (float) curWidth / curHeight, 0.2f, 20.0f);
			hvInfo("Selected projection: PERSPECTIVE");
			break;
		case 'f':
			hvModelFaceFlip(&letterH);
			hvModelSync(&letterH);
			hvModelFaceFlip(&letterV);
			hvModelSync(&letterV);
			break;
		case 'r':
			hvModelFaceRoll(&letterH);
			hvModelSync(&letterH);
			hvModelFaceRoll(&letterV);
			hvModelSync(&letterV);
	}
	glUseProgram(shaderProgram.id);
		glUniformMatrix4fv(shaderProgram.projMatrixUniLoc, 1, GL_FALSE, projectionMatrix.m);
	glUseProgram(0);
}

void hvaSpecial(int key, int x, int y) {
	x = y; y = x;
	if (key == GLUT_KEY_LEFT)
		camX -= .1f;
	else if (key == GLUT_KEY_RIGHT)
		camX += .1f;
	else if (key == GLUT_KEY_UP)
		camZ -= .1f;
	else if (key == GLUT_KEY_DOWN)
		camZ += .1f;
	hvMatrixViewCamera(&viewMatrix, vec(camX, 0, camZ), vec(0, 0, 0), vec(0, 1, 0));
	hvDebug("View matrix preview:");
	hvMatrixPrintFC(&viewMatrix);
}

void hvaClean() {
	hvDeleteProgram(&shaderProgram);
	hvDeleteModel(&letterH);
	hvDeleteModel(&letterV);
	hvInfo("Resources released");
}

/*
	Removed function calls:
		glDepthFunc(GL_LESS);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVA);

	Alternative API concept:
		hvBegin(HV_MODEL);
			hvStockMesh(HV_MODEL_LETTERH);
			hvPaintSolid(.8f, .2f, .2f);
		hvEnd(HV_MODEL);
*/
