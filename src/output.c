
#include "output.h"
#include "output_p.h"

// TODO: use STDERR (two stream levels)
// TODO: external string resources with encryption
// TODO: strnfun(char** dst, const char * const end, const char*);

// Message concatenation macros
#define hvLogMsgBegin() \
	log.offset = 0

#define hvLogMsgStr(str) \
	strcpy(log.buffer + log.offset, str); \
	log.offset += strlen(str)

#define hvLogMsgGet(buf) \
	log.buffer[log.offset++] = '\0'; \
	strcpy(buf, log.buffer);

#define hvLogMsgEnd() \
	log.buffer[log.offset] = '\n'; \
	log.buffer[++log.offset] = '\0'; \
	fputs(log.buffer, log.target)

#define ESC "\x1B"

// ANSI VT100 escape sequences for setting display attributes
static const char *hvLogColourCode[] = {
	ESC"[0m", ESC"[31m", ESC"[32m", ESC"[34m", ESC"[33m", ESC"[37;2m"
};

// ESC [ 3 g  Clears all horizontal tab stops
// ESC H      Sets a horizontal tab stop at cursor position
// ESC [ * C  Moves cursor right * columns.

// HV logging context
static struct {
	char* buffer;
	size_t offset;
	enum log_e level;
	unsigned char numCur; 
	char* numBuf[HV_BUFS_NUM];
	unsigned char strCur;
	char* strBuf[HV_BUFS_STR];
	char* logBuf;
	FILE* target;
} log;

// Log (re)initialisation
void hvLogInit(enum log_e level, FILE* target) {
	log.level  = level;
	log.target = target;
	log.offset = 0;
	log.numCur = 0;
	log.strCur = 0;
	if (log.buffer == NULL) {
		log.buffer = (char*) malloc(HV_SIZE_MSG);
		log.logBuf = (char*) malloc(HV_SIZE_LOG);
		for (unsigned char i = 0; i < HV_BUFS_NUM; ++i)
			log.numBuf[i] = (char*) malloc(HV_SIZE_NUM);
		for (unsigned char i = 0; i < HV_BUFS_STR; ++i)
			log.strBuf[i] = (char*) malloc(HV_SIZE_STR);
	}
}

// Log termination
void hvLogTerm() {
	if ((log.numCur | log.strCur) != 0)
		hvWarning("Log termination with not-empty message buffers.");
	if (log.buffer != NULL) {
		free(log.buffer);
		free(log.logBuf);
		for (unsigned char i = 0; i < HV_BUFS_NUM; ++i)
			free(log.numBuf[i]);
		for (unsigned char i = 0; i < HV_BUFS_STR; ++i)
			free(log.strBuf[i]);
	}
}

#if HV_DEBUG_LEVEL == HV_EXTENDED

	#include <limits.h>
	#include <float.h>

	void hvLogLimits() {
		hvDebug("Sizes of integer types:");
		hvPrint("\tCHAR_MIN:  ", hvInt(CHAR_MIN));
		hvPrint("\tCHAR_MAX:  ", hvInt(CHAR_MAX));
		hvPrint("\tUCHAR_MAX: ", hvNum(UCHAR_MAX));
		hvPrint("\tUSHRT_MAX: ", hvNum(USHRT_MAX));
		hvPrint("\tUINT_MAX:  ", hvNum(UINT_MAX));
		hvPrint("\tULONG_MAX: ", hvNum(ULONG_MAX));
	}

	void hvLogFloats() {
		hvDebug("Characteristics of floating types");
		hvPrint("\tFLT_ROUNDS: ", hvInt(FLT_ROUNDS));
		hvPrint("\tFLT_EVAL_METHOD: ", hvInt(FLT_EVAL_METHOD));
		hvPrint("\tFLT_DIG: ", hvInt(FLT_DIG));
		snprintf(log.numBuf[log.numCur], HV_SIZE_NUM, "%10.8f ", FLT_EPSILON);
		hvPrint("\tFLT_EPSILON: ", log.numBuf[log.numCur]);
	}

#else
	
	void hvLogLimits() { }
	void hvLogFloats() { }

#endif

const char* hvInt(int num) {
	snprintf(log.numBuf[log.numCur], HV_SIZE_NUM, "%d", num);
	return log.numBuf[log.numCur++];
}

const char* hvHex(int num) {
	snprintf(log.numBuf[log.numCur], HV_SIZE_NUM, "0x%x", num);
	return log.numBuf[log.numCur++];
}

const char* hvBin(int num) {
	char * dig = log.numBuf[log.numCur];
	for (unsigned int pos = 1U << 31; pos > 0; pos >>= 1)
		*dig++ = (num & pos) ? '1' : '0';
	*dig = '\0';
	return log.numBuf[log.numCur++];
}

const char* hvNum(size_t num) {
	snprintf(log.numBuf[log.numCur], HV_SIZE_NUM, "%zu", num);
	return log.numBuf[log.numCur++];
}

const char* hvFlt(float num) {
	snprintf(log.numBuf[log.numCur], HV_SIZE_NUM, "%5.2f ", num);
	return log.numBuf[log.numCur++];
}

void hvLogTabWidth(unsigned char width) {
	#ifdef __gnu_linux__
	if (log.target == stdout) {
		static char command[48];
		sprintf(command, "setterm -term linux -regtabs %d", width);
		if (system(command) != -1)
			hvDebug("Terminal tab-stop pattern set to +", hvInt(width));
		else
			hvWarning("Cannot set terminal tab-stop pattern");
	}
	#endif
}

const char* hvLogString(const unsigned char argc, ...) {
	assert(argc <= HV_BUFS_NUM);
	va_list argv;
	const char* str;
	hvLogMsgBegin();
		va_start(argv, argc);
		for (unsigned char i = 0; i < argc; ++i) {
			str = va_arg(argv, const char*);
			hvLogMsgStr(str);
		}
		va_end(argv);
	hvLogMsgGet(log.strBuf[log.strCur]);
	log.numCur = 0;
	return log.strBuf[log.strCur++];	
}

const char* hvLogColour(enum col_e colour, const unsigned char argc, ...) {
	va_list argv;
	const char* str;
	hvLogMsgBegin();
		#ifdef __gnu_linux__
		hvLogMsgStr(hvLogColourCode[colour]);
		#endif
		va_start(argv, argc);
		for (unsigned char i = 0; i < argc; ++i) {
			str = va_arg(argv, const char*);
			hvLogMsgStr(str);
		}
		va_end(argv);
		#ifdef __gnu_linux__
		hvLogMsgStr(hvLogColourCode[HV_LOG_PLAIN]);
		#endif
	hvLogMsgGet(log.strBuf[log.strCur]);
	log.numCur = 0;
	return log.strBuf[log.strCur++];
}

void hvLogMessage(enum log_e level, const unsigned char argc, ...) {
	if (log.level >= level) {
		va_list argv;
		const char* string;
		va_start(argv, argc);
		hvLogMsgBegin();
			hvLogMsgStr(hvLogLabel[level]);
			for (unsigned char i = 0; i < argc; ++i) {
				string = va_arg(argv, const char*);
				hvLogMsgStr(string);
			}
		hvLogMsgEnd();
		va_end(argv);
	}
	log.numCur = 0;
	log.strCur = 0;
}

void hvLogErrorCheck(bool cond, const unsigned char argc, ...) {
	if (cond) {
		va_list argv;
		const char* string;
		va_start(argv, argc);
		hvLogMsgBegin();
			hvLogMsgStr(" ERROR: ");
			for (unsigned char i = 0; i < argc; ++i) {
				string = va_arg(argv, const char*);
				hvLogMsgStr(string);
			}
		hvLogMsgEnd();
		va_end(argv);
		exit(EXIT_FAILURE);
	}
	log.numCur = 0;
}

#ifdef HV_DS1_OPENGL_H
void hvErrorCheckGL(const char* message) {
	const GLenum gl_error = glGetError();
	if (gl_error != GL_NO_ERROR) {
		if (log.level >= HV_LOG_ERROR) {
			hvLogMsgBegin();
				hvLogMsgStr("  ERROR: ");
				hvLogMsgStr(message);
				hvLogMsgStr(": ");
				hvLogMsgStr(hvStr(gluErrorString(gl_error)));
			hvLogMsgEnd();
		}
		free(log.buffer);
		exit(EXIT_FAILURE);
	}
}

void hvErrorCheckGLSL(GLuint name) {
	// TODO: checking for plain GL errors
	GLint params = GL_FALSE;
	if (glIsShader(name) == GL_TRUE) {
		glGetShaderiv(name, GL_COMPILE_STATUS, &params);
		if (params == GL_TRUE)
			return;
		if (log.level >= HV_LOG_ERROR) {
			hvLogMsgBegin();
				hvLogMsgStr("  ERROR: ");
				glGetShaderiv(name, GL_SHADER_TYPE, &params);
				hvLogMsgStr(hvTypeGLSL(params));
				hvLogMsgStr(" shader compilation error");
				glGetShaderiv(name, GL_INFO_LOG_LENGTH, &params);
				hvLogMsgStr((params > 1) ? ":" : " (no info log).");
			hvLogMsgEnd();
			if (params > 1) {
				hvLogMsgBegin();
					glGetShaderInfoLog(name, HV_SIZE_LOG, NULL, log.logBuf);
					hvLogMsgStr("\t");
					hvLogMsgStr(log.logBuf);
				hvLogMsgEnd();
			}
		}	
	}
	else if (glIsProgram(name) == GL_TRUE) {
		glGetProgramiv(name, GL_LINK_STATUS, &params);
		if (params == GL_TRUE)
			return;
		if (log.level >= HV_LOG_ERROR) {
			hvLogMsgBegin();
				hvLogMsgStr("  ERROR: ");
				hvLogMsgStr("Shader program linking error");
				glGetProgramiv(name, GL_INFO_LOG_LENGTH, &params);
				hvLogMsgStr((params > 1) ? ":" : " (no info log).");
			hvLogMsgEnd();
			if (params > 1) {
				hvLogMsgBegin();
					glGetProgramInfoLog(name, HV_SIZE_LOG, NULL, log.logBuf);
					hvLogMsgStr("\t");
					hvLogMsgStr(log.logBuf);
				hvLogMsgEnd();
			}
		}	
	}
	else {
		hvLogMsgBegin();
			hvLogMsgStr("  LOG_ERROR: ");
			hvLogMsgStr("unsupported object type for hvErrorCheckGLSL(GLint name).");
		hvLogMsgEnd();
	}
	free(log.buffer);
	exit(EXIT_FAILURE);
}

const char* hvIntGL(GLint value) {
	switch(value) {
		case GL_TRUE:
			return "GL_TRUE";
		case GL_FALSE:
			return "GL_FALSE";
		default:
			return "[unknown]";
	}
}

const char* hvTypeGL(GLint type) {
	switch(type) {
		case GL_INT:
			return "GL_INT";
		case GL_BOOL:
			return "GL_BOOL";
		case GL_FLOAT:
			return "GL_FLOAT";
		case GL_FLOAT_VEC2:
			return "GL_FLOAT_VEC2";
		case GL_FLOAT_VEC3:
			return "GL_FLOAT_VEC3";
		case GL_FLOAT_VEC4:
			return "GL_FLOAT_VEC4";
		case GL_FLOAT_MAT2:
			return "GL_FLOAT_MAT2";
		case GL_FLOAT_MAT3:
			return "GL_FLOAT_MAT3";
		case GL_FLOAT_MAT4:
			return "GL_FLOAT_MAT4";
		default:
			return "[unknown]";
	}	
}

const char* hvTypeGLSL(GLint type) {
	switch (type) {
		case GL_VERTEX_SHADER:
			return "Vertex";
		case GL_FRAGMENT_SHADER:
			return "Fragment";
		case GL_GEOMETRY_SHADER:
			return "Geometry";
		default:
			return "[unknown]";
	}
}

#endif

/*
 *  HV/output/frame
 */

// Multi-column output buffer
static struct {
	char
		*buffer,
		separator;
	unsigned char
		width, height,
		col_offset,
		row_offset,
		padding;
} frame;

void hvFrameInit(unsigned char width, unsigned char height) {
	frame.width = width;
	frame.height = height;
	frame.col_offset = 0;
	frame.row_offset = 0;
	frame.padding = 0;
	frame.separator = '\0';
	frame.buffer = (char*) malloc(sizeof(char) * width * height);
	// TODO: use memset: void *memset(void *s, int c, size_t n);
	for (unsigned char row = 0; row < frame.height; ++row) {
		*(frame.buffer + row * frame.width) = '\0';
		for (unsigned char col = 1; col < frame.width; ++col)
			*(frame.buffer + row * frame.width + col) = ' ';
	}
}

void hvFrameColumnSeparator(const char separator) {
	frame.separator = separator;
}

void hvFrameColumnPadding(unsigned char width) {
	if (width < 4)
		frame.padding = width;
}

void hvFramePrint() {
	for (unsigned char row = 0; row < frame.height; ++row)
		puts(frame.buffer + frame.width * row);
	free(frame.buffer);
}

void hvFrameDebug() {
	char cur_chr;
	for (unsigned char row = 0; row < frame.height; ++row) {
		for (unsigned char col = 0; col < frame.width; ++col) {
			cur_chr = *(frame.buffer + row * frame.width + col);
			if (cur_chr == ' ')
				putc('.', stdout);
			else if (cur_chr == '\0') 
				putc('0', stdout);
			else
				putc(cur_chr, stdout);
		}
		putc('\n', stdout);
	}
	putc('\n', stdout);
}

void hvFrameRowPuts(const char* str) {
	char* buf_ptr = frame.buffer + frame.row_offset * frame.width + frame.col_offset;
	strcpy(buf_ptr, str);
	if (frame.row_offset + 1 < frame.height)
		frame.row_offset += 1;
	else
		hvFrameColumnNext();
}

void hvFrameRowSkip(unsigned char count) {
	if (frame.row_offset + count < frame.height)
		frame.row_offset += count;	
	else {
		unsigned char row_dif = frame.row_offset + count - frame.height;
		hvFrameColumnNext();
		hvFrameRowSkip(row_dif);
	}
}

void hvFrameColumnNext() {
	char* buf_ptr = 0;
	unsigned char cur_len = 0;
	unsigned char max_len = 0;
	
	for (unsigned char row = 0; row < frame.height; ++row) {
		buf_ptr = frame.buffer + frame.width * row + frame.col_offset;
		cur_len = (unsigned char) strlen(buf_ptr);
		if (cur_len > max_len)
			max_len = cur_len;
	}

	for (unsigned char row = 0; row < frame.height; ++row) {
		buf_ptr = frame.buffer + row * frame.width + frame.col_offset;
		cur_len = (unsigned char) strlen(buf_ptr);
		 buf_ptr += cur_len;
		*buf_ptr = ' ';
		 buf_ptr += max_len - cur_len;
		 if (frame.padding > 0)
		 	for (unsigned char i = 0; i < frame.padding; ++i)
		 		*buf_ptr++ = ' ';
		 if (frame.separator != '\0')
		 	*buf_ptr++ = frame.separator;
		 if (frame.padding > 0)
		 	for (unsigned char i = 0; i < frame.padding; ++i)
		 		*buf_ptr++ = ' ';		 	
		*buf_ptr = '\0';
	}
	frame.row_offset = 0;
	frame.col_offset += max_len + 2 * frame.padding + (frame.separator ? 1 : 0);
}

void hvFrameTerm() {
	free(frame.buffer);
}

