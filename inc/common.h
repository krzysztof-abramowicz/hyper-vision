/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Application Framework
 *     content: Common configuration and declarations header (text/x-h)
 *     version: 1.0 OS
 *
 * The HyperVision DS1 code is meant to be strictly conforming to the ISO/IEC 8988:1999 standard.
 *
 * The HV API is based on OpenGL conventions, particularly in regard to:
 *   • function names (with strict naming hierarchy, excluding `Create` and `Delete`),
 *   • function argumets names, types and order.
 */

#ifndef HV_DS1_COMMON_H
#define HV_DS1_COMMON_H

#include "define.h"

/* ## Configuration section ## */

/* Trigonometric functions implementation:
 *   • HV_CLIB — use C standard library functions,
 *   • HV_LTAB — use hard-coded lookup tables.
 */ 
#define HV_FUNCTION_MODE HV_LTAB

/* Linear arithmetic implementation:
 *   • HV_SCALAR — perform classic scalar computation (C),
 *   • HV_VECTOR — use vectorised algorithms (SSE intrinsics),
 */
#define HV_OPERATION_MODE HV_SCALAR

/* Available debugging facilities:
 *   • HV_STANDARD — essential logging and error handling (production build),
 *   • HV_EXTENDED — verbose output and detailed reporting (testing build).
 */
#define HV_DEBUG_LEVEL HV_EXTENDED

/* Experimental x86-64 assembly optimisations (dissuaded) */
//#define HV_ASSEMBLY_HACKS


/* ## Assertions' section ## */

#if __STDC_VERSION__ < 199901L
	#error The source code requires a C'99 compiler (C'99 mode enabled)
#endif

#ifndef __STDC_IEC_559__
	#error The source code expects an IEC 60559 conforming compiler
#endif

#if HV_OPERATION_MODE == HV_VECTOR && !(__SSE__ || __x86_64__)
	#error Linear arithmetic vectorisation requires SSE-enabled target architecture
#endif

#ifndef __gnu_linux__
	#pragma message("GNU/Linux extensions are excluded for this build")
#endif


/* ## Definitions' section ## */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if HV_DEBUG_LEVEL != HV_EXTENDED
	#define NDEBUG
#endif
#include <assert.h>

/* A type for passing argument count for variadic functions
 *  + cannot have trap representation (as being _unsigned char_)
 *  + cannot overflow or be negative (qua _unsigned integer type_)
 *  - may exceed 127 -- function's arguments limit in C99
 */
typedef const unsigned char argc_t;

#endif /* defined HV_DS1_COMMON_H */

