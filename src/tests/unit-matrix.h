/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Unit tests
 *     toolset: ISO C '99 + GAS + glib
 *     content: Declarations for the [matrix] module test cases (text/x-h)
 *     version: 1.0
 */

#include "output.h"
#include "matrix.h"
#include <xmmintrin.h>

#if HV_FUNCTION_MODE != HV_LTAB
	#error The test requires [matrix] module with look-up tables optimisation enabled.
#endif

/* Test-local hvMatrixMultipliply implementation variants (def: matrix-impl.c, matrix-impl.s) */
void hvMatrixMultiply_std(const mat4_t *A, const mat4_t *B, mat4_t *C);
void hvMatrixMultiply_sse(const mat4_t *A, const mat4_t *B, mat4_t *C);
void hvMatrixMultiply_asm(const mat4_t *A, const mat4_t *B, mat4_t *C);

/* Test-local hvMatrixRotateX implementation variants (def: matrix-impl.c) */
void hvMatrixRotateX_std_clib(mat4_t *mat, float angle);
void hvMatrixRotateX_std_ltab(mat4_t *mat, float angle);
void hvMatrixRotateX_sse_clib(mat4_t *mat, float angle);
void hvMatrixRotateX_sse_ltab(mat4_t *mat, float angle);
void hvMatrixRotateX_sse_ltab_iv(mat4_t *mat, float angle);

/* Test-local hvMatrixViewCamera implementation variants (def: matrix-impl.c) */
typedef struct { float v[3]; } vec3_s;
void hvMatrixViewCamera_std(mat4_t *view, vec3_s cam, vec3_s ref, vec3_s up);
void hvMatrixViewCamera_sse(mat4_t *view, __m128 cam, __m128 ref, __m128 up);

/* Test-local hvMatrixIdentity definitions */
static inline void hvMatrixIdentity_std(mat4_t * mat) {
	static const mat4_t IDENTITY = {{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}};
	memcpy(mat->m, IDENTITY.m, 16 * sizeof(float));
}

static inline void hvMatrixIdentity_sse(mat4_t * mat) {
	static const mat4_t IDENTITY = {{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}};
	_mm_store_ps(&mat->m[ 0], _mm_load_ps(&IDENTITY.m[ 0]));
	_mm_store_ps(&mat->m[ 4], _mm_load_ps(&IDENTITY.m[ 4]));
	_mm_store_ps(&mat->m[ 8], _mm_load_ps(&IDENTITY.m[ 8]));
	_mm_store_ps(&mat->m[12], _mm_load_ps(&IDENTITY.m[12]));
}

