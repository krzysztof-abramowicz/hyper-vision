
#include "opengl.h"

#ifdef GLUT_API_VERSION
void hvAppInitWindow(int width, int height, char* title) {
	int argc = 1;
	char *argv[] = { "HyperVision", NULL };
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(300, 100);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	int handle = glutCreateWindow(title);
	hvErrorCheck(handle < 1, "Could not create a new rendering window.");
}

void hvAppEvent(enum event_e event, void (*func)()) {
	switch (event) {
		case HV_RESHAPE:
			glutReshapeFunc(func);
			break;
		case HV_DISPLAY:
			glutDisplayFunc(func);
			break;
		case HV_KEYBOARD:
			glutKeyboardFunc((void (*)(unsigned char, int, int)) func);
			break;
		case HV_SPECIAL:
			glutSpecialUpFunc((void (*)( int, int, int ))func);
			break;
		case HV_IDLE:
			glutIdleFunc((void (*)()) func); 
	}
}
#endif /* defined GLUT_API_VERSION */

#ifdef GLFW_VERSION_MAJOR
GLFWwindow* window;

void hvAppInitWindow(int width, int height, char* title) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwMakeContextCurrent(window);
}

void hvAppCallback(enum event_e event, void (*func)()) {
	switch (event) {
		case HV_RESHAPE:
			glfwSetWindowSizeCallback(window, func); break;
		case HV_DISPLAY:
			glfwSetWindowRefreshCallback(window, func); break;
		case HV_IDLE:
	}
	
#endif /* defined GLFW_VERSION_MAJOR */

void hvAppInitCanvas() {
	/* Load OpenGL API declarations */
	glewExperimental = GL_TRUE;
	GLenum result = glewInit();
	hvErrorCheck(result != GLEW_OK, "Cannot initialise GLEW: ", hvStr(glewGetErrorString(result)));

	/* Prist version information */
	hvInfo("Detected OpenGL version: ", hvStr(glGetString(GL_VERSION)));
	GLint maj, min;
	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);
	hvErrorCheck(maj * 10 + min < 33, "Available OpenGL version is to low (required 3.3 or higher)");
	hvInfo("Selected OpenGL version: 3.3.0 CORE PROFILE");

	/* Report target device */
	hvInfo("OpenGL rendering device: ", hvStr(glGetString(GL_RENDERER)));

	glGetError();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	hvErrorCheckGL("Could not enable OpenGL depth buffer");

	glEnable(GL_CULL_FACE);
	hvErrorCheckGL("Could not enable face culling");
}

