
#include "shader.h"
#include "shader_p.h"

prog_t hvCreateProgram(enum vs_e vertShader, enum fs_e fragShader)
{
	prog_t prog = { 0 };
	
	prog.id = glCreateProgram();
	hvErrorCheck(prog.id == 0, "Could not create the shader programme");

	prog.vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(prog.vs, 1, stockVertexShader[vertShader], NULL);
	glCompileShader(prog.vs);
	hvErrorCheckGLSL(prog.vs);

	prog.fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(prog.fs, 1, stockFragmentShader[fragShader], NULL);
	glCompileShader(prog.fs);
	hvErrorCheckGLSL(prog.fs);

	glAttachShader(prog.id, prog.vs);
	glAttachShader(prog.id, prog.fs);
	glLinkProgram(prog.id);
	hvErrorCheckGLSL(prog.id);

    //glDeleteShader(prog.vs);
    //glDeleteShader(prog.fs);

	prog.mdelMatrixUniLoc = glGetUniformLocation(prog.id, "ModelMatrix");
	prog.viewMatrixUniLoc = glGetUniformLocation(prog.id, "ViewMatrix");
	prog.projMatrixUniLoc = glGetUniformLocation(prog.id, "ProjectionMatrix");
	prog.colorUniLoc = glGetUniformLocation(prog.id, "hv_Color");
	hvErrorCheckGL("Could not get the shader programme uniform locations");

	return prog;
}

void hvProgramInfo(prog_t* prog)
{
	hvDebug("Shader programme info:");
	GLint param = GL_FALSE;
	glGetProgramiv(prog->id, GL_LINK_STATUS, &param);
	hvPrint("\tGL_LINK_STATUS = ", hvIntGL(param));
	glGetProgramiv (prog->id, GL_ATTACHED_SHADERS, &param);
	hvPrint("\tGL_ATTACHED_SHADERS = ", hvInt(param));
	GLenum type;
	int size = 0;
	int length = 0;
	char name[HV_NAME_SIZE];
	/* Present active attributes' list */
	glGetProgramiv(prog->id, GL_ACTIVE_ATTRIBUTES, &param);
	hvPrint("\tGL_ACTIVE_ATTRIBUTES = ", hvInt(param));
	for (GLint i = 0; i < param; ++i) {
		glGetActiveAttrib(prog->id, i, HV_NAME_SIZE, &length, &size, &type, name);
		int loc = glGetAttribLocation (prog->id, name);
		printf("\t\t#%i loc:%i type:%s name:%s\n", i, loc, hvTypeGL(type), name);
	}
	/* Present active uniforms' list */
	glGetProgramiv(prog->id, GL_ACTIVE_UNIFORMS, &param);
	hvPrint("\tGL_ACTIVE_UNIFORMS = ", hvInt(param));
	for (GLint i = 0; i < param; ++i) {
		glGetActiveUniform(prog->id, i, HV_NAME_SIZE, &length, &size, &type, name);
		int loc = glGetUniformLocation(prog->id, name);
		printf("\t\t#%i loc:%i type:%s name:%s \n", i, loc, hvTypeGL(type), name);
	}
	/* Print programme's info log */
	glGetProgramiv(prog->id, GL_INFO_LOG_LENGTH, &param);
	hvPrint("\tGL_INFO_LOG_LENGTH = ", hvInt(param));
	if (param > 1) {
		char log[HV_INFO_SIZE];
		glGetProgramInfoLog(prog->id, HV_INFO_SIZE, &length, log);
		printf("\tprogram info log for GL index %i:\n%s", prog->id, log);
	}
}

void hvDeleteProgram(prog_t* prog)
{
	glDetachShader(prog->id, prog->vs);
	glDetachShader(prog->id, prog->fs);
	glDeleteShader(prog->vs);
	glDeleteShader(prog->fs);
	glDeleteProgram(prog->id);
	prog->mdelMatrixUniLoc = 0;
	prog->viewMatrixUniLoc = 0;
	prog->projMatrixUniLoc = 0;
}

