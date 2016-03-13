/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Design approach test program 1 (DATP_1)
 *                  └─Unit tests
 *     toolset: ISO C '99 + GAS + glib
 *     content: Test-local functions' implementation variants (text/x-c)
 *     version: 1.0
 */

#include "unit-matrix.h"
#include "../matrix_p.h"

/* hvMatrixMultiply: implementation using standard computation */
void hvMatrixMultiply_std(const mat4_t *A, const mat4_t *B, mat4_t *C) {
	for (unsigned int i = 0; i < 16; i += 4)
		for (unsigned int j = 0; j < 4; j++)
			C->m[i + j] = (B->m[i + 0] * A->m[j +  0])
			            + (B->m[i + 1] * A->m[j +  4])
			            + (B->m[i + 2] * A->m[j +  8])
			            + (B->m[i + 3] * A->m[j + 12]);
}

/* hvMatrixMultiply: implementations using SSE intrinsics */

void hvMatrixMultiply_sse(const mat4_t *A, const mat4_t *B, mat4_t *C) {
	__m128 row1 = _mm_load_ps(&A->m[0]);
	__m128 row2 = _mm_load_ps(&A->m[4]);
	__m128 row3 = _mm_load_ps(&A->m[8]);
	__m128 row4 = _mm_load_ps(&A->m[12]);
	for(unsigned int i=0; i<4; i++) {
		__m128 brod1 = _mm_set1_ps(B->m[4*i+0]);
		__m128 brod2 = _mm_set1_ps(B->m[4*i+1]);
		__m128 brod3 = _mm_set1_ps(B->m[4*i+2]);
		__m128 brod4 = _mm_set1_ps(B->m[4*i+3]);
		__m128 row =
		_mm_add_ps(
			_mm_add_ps(
				_mm_mul_ps(brod1, row1),
				_mm_mul_ps(brod2, row2)),
			_mm_add_ps(
				_mm_mul_ps(brod3, row3),
				_mm_mul_ps(brod4, row4))
		);
		_mm_store_ps(&C->m[4*i], row);
    }
}

static inline void hvMatrixMultiplyI_sse(const mat4_t *A, mat4_t *B) {
		__m128 row1 = _mm_load_ps(&A->m[0]);
		__m128 row2 = _mm_load_ps(&A->m[4]);
		__m128 row3 = _mm_load_ps(&A->m[8]);
		__m128 row4 = _mm_load_ps(&A->m[12]);
		for(unsigned int i = 0; i < 4; ++i) {
			__m128 brod1 = _mm_set1_ps(B->m[4*i+0]);
			__m128 brod2 = _mm_set1_ps(B->m[4*i+1]);
			__m128 brod3 = _mm_set1_ps(B->m[4*i+2]);
			__m128 brod4 = _mm_set1_ps(B->m[4*i+3]);
			__m128 row =
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(brod1, row1),
						_mm_mul_ps(brod2, row2)),
					_mm_add_ps(
						_mm_mul_ps(brod3, row3),
						_mm_mul_ps(brod4, row4))
				);
			_mm_store_ps(&B->m[4*i], row);
		}
}

static inline void hvMatrixMultiplyIV_sse(__m128 v0, __m128 v1, __m128 v2, __m128 v3, mat4_t *B) {
	for (unsigned int i = 0; i < 4; ++i)
	_mm_store_ps(
		&B->m[4*i],
		_mm_add_ps(
			_mm_add_ps(
				_mm_mul_ps(v0, _mm_set1_ps(B->m[4*i+0])) ,
				_mm_mul_ps(v1, _mm_set1_ps(B->m[4*i+1])) ) ,
			_mm_add_ps(
				_mm_mul_ps(v2, _mm_set1_ps(B->m[4*i+2])) ,
				_mm_mul_ps(v3, _mm_set1_ps(B->m[4*i+3])) ) ) );
}

/* hvRotateX: implementation using standard computation */
void hvMatrixRotateX_std_clib(mat4_t * restrict mat, float angle) {
	mat4_t rotate;
	hvMatrixIdentity_std(&rotate);
	float sine   = sin(angle * 0.017453f);
	float cosine = cos(angle * 0.017453f);
	rotate.m[Yy] =  cosine;
	rotate.m[Yz] =  sine;
	rotate.m[Zy] = -sine;
	rotate.m[Zz] =  cosine;
	mat4_t tmp_mat;	
	hvMatrixMultiply_std(&rotate, mat, &tmp_mat);
	memcpy(mat->m, &tmp_mat, sizeof(mat4_t));
}

/* hvRotateX: implementation using look-up tables */
void hvMatrixRotateX_std_ltab(mat4_t * restrict mat, float angle) {
	mat4_t rotate;
	hvMatrixIdentity_std(&rotate);
	float sine   = sin_ltab[(unsigned int) angle];
	float cosine = cos_ltab[(unsigned int) angle];
	rotate.m[Yy] =  cosine;
	rotate.m[Yz] =  sine;
	rotate.m[Zy] = -sine;
	rotate.m[Zz] =  cosine;
	mat4_t tmp_mat;	
	hvMatrixMultiply_std(&rotate, mat, &tmp_mat);
	memcpy(mat->m, &tmp_mat, sizeof(mat4_t));
}

/* hvRotateX: implementation using SSE intrinsics */
void hvMatrixRotateX_sse_clib(mat4_t * restrict mat, float angle) {
	mat4_t rotate;
	hvMatrixIdentity_sse(&rotate);
	float sine   = sin(angle * 0.017453f);
	float cosine = cos(angle * 0.017453f);
	rotate.m[Yy] =  cosine;
	rotate.m[Yz] =  sine;
	rotate.m[Zy] = -sine;
	rotate.m[Zz] =  cosine;
	hvMatrixMultiplyI_sse(&rotate, mat);
}

/* hvRotateX: implementation using SSE intrinsics and look-up tables */
void hvMatrixRotateX_sse_ltab(mat4_t * restrict mat, float angle) {
	mat4_t rotate;
	hvMatrixIdentity_sse(&rotate);
	float sine   = sin_ltab[(unsigned int) angle];
	float cosine = cos_ltab[(unsigned int) angle];
	rotate.m[Yy] =  cosine;
	rotate.m[Yz] =  sine;
	rotate.m[Zy] = -sine;
	rotate.m[Zz] =  cosine;
	hvMatrixMultiplyI_sse(&rotate, mat);
}

/* hvRotateX: experimental implementation */
static float __attribute__((aligned(16))) HV_IMX[] = {1.0f, 0.0f, 0.0f, 0.0f};
static float __attribute__((aligned(16))) HV_IMT[] = {0.0f, 0.0f, 0.0f, 1.0f};
void hvMatrixRotateX_sse_ltab_iv(mat4_t * restrict mat, float angle) {
	float sine   = sin_ltab[(unsigned int) angle];
	float cosine = cos_ltab[(unsigned int) angle];
	__m128 x = _mm_load_ps(HV_IMX);
	__m128 y = {0.0f, cosine, sine, 0.0f};
	__m128 z = {0.0f, -sine, cosine, 0.0f};
	__m128 t = _mm_load_ps(HV_IMT);
	hvMatrixMultiplyIV_sse(x, y, z, t, mat);
}

/* hvMatrixViewCamera: implementation using standard computation */
extern inline vec3_s hvVectorSub_std(vec3_s a, vec3_s b) {
		return (vec3_s) {{
			a.v[0] - b.v[0],
			a.v[1] - b.v[1],
			a.v[2] - b.v[2]
		}};
}
extern inline float hvVectorLen_std(vec3_s vec) {
	return sqrt(
		vec.v[0] * vec.v[0] +
		vec.v[1] * vec.v[1] +
		vec.v[2] * vec.v[2]
	);
}
extern inline vec3_s hvVectorNorm_std(vec3_s vec) {
	float len = hvVectorLen_std(vec);
	if (len != 0.0f)
		return (vec3_s){{
			vec.v[0] / len,
			vec.v[1] / len,
			vec.v[2] / len
		}};
	else
		return (vec3_s){{0.0f, 0.0f, 0.0f}};
}
extern inline vec3_s hvVectorCross_std(vec3_s a, vec3_s b) {
	return (vec3_s) {{
		a.v[Y] * b.v[Z] - a.v[Z] * b.v[Y],
		a.v[Z] * b.v[X] - a.v[X] * b.v[Z],
		a.v[X] * b.v[Y] - a.v[Y] * b.v[X]
	}};
}
void hvMatrixViewCamera_std(mat4_t * restrict view, vec3_s cam, vec3_s ref, vec3_s up) {
	mat4_t trans;
	hvMatrixIdentity_std(&trans);
	trans.m[Tx] = -cam.v[X];
	trans.m[Ty] = -cam.v[Y];
	trans.m[Tz] = -cam.v[Z];
	vec3_s u = hvVectorNorm_std(up);
	vec3_s f = hvVectorNorm_std(hvVectorSub_std(ref, cam));
	vec3_s s = hvVectorNorm_std(hvVectorCross_std(f, u));
	u = hvVectorCross_std(s, f);
	mat4_t rotat;
	hvMatrixIdentity_std(&rotat);
	rotat.m[Xx] =  s.v[X];
	rotat.m[Yx] =  s.v[Y];
	rotat.m[Zx] =  s.v[Z];
	rotat.m[Xy] =  u.v[X];
	rotat.m[Yy] =  u.v[Y];
	rotat.m[Zy] =  u.v[Z];
	rotat.m[Xz] = -f.v[X];
	rotat.m[Yz] = -f.v[Y];
	rotat.m[Zz] = -f.v[Z];
	hvMatrixMultiply_std(&rotat, &trans, view);
}

/* hvMatrixViewCamera: implementation using SSE intrinsics */
extern inline __m128 hvVectorNeg_sse(__m128 a) {
	return _mm_xor_ps(a, _mm_castsi128_ps(
		_mm_set1_epi32(1U << 31)
	));
}
extern inline __m128 hvVectorSub_sse(__m128 a, __m128 b) {
	return _mm_sub_ps(a, b);
}
extern inline float hvVectorLen_sse(__m128 vec) {
	vec = _mm_mul_ps(vec, vec);
	vec = _mm_add_ps(vec, _mm_shuffle_ps(vec, vec, 0xB1));
	vec = _mm_add_ps(vec, _mm_shuffle_ps(vec, vec, 0x1B));
	vec = _mm_sqrt_ss(vec);
	return _mm_cvtss_f32(vec);
}
extern inline __m128 hvVectorNorm_sse(__m128 vec) {
	float len = hvVectorLen_sse(vec);
	if ( __builtin_expect(len != 0.0f, 1) )
		return _mm_div_ps(vec, _mm_set1_ps(len)); //? multiply by reciprocal
	else
		return (__m128){0.0f, 0.0f, 0.0f, 0.0f};
}
extern inline __m128 hvVectorCross_sse(__m128 a, __m128 b) {
	__m128 res =
		_mm_sub_ps(
			_mm_mul_ps(a, _mm_shuffle_ps(b, b, 0xC9)),
			_mm_mul_ps(b, _mm_shuffle_ps(a, a, 0xC9))
		);
	return _mm_shuffle_ps(res, res, 0xC9);
}
void hvMatrixViewCamera_sse(mat4_t * restrict view, __m128 cam, __m128 ref, __m128 up) {
	mat4_t T;
	hvMatrixIdentity_sse(&T);
	_mm_store_ps(&T.m[Tx], hvVectorNeg_sse(cam));
	T.m[Tw] = 1.0f;
	__m128 f = hvVectorNorm_sse(hvVectorSub_sse(ref, cam));
	__m128 s = hvVectorNorm_sse(hvVectorCross_sse(f, up));	
	__m128 u = hvVectorCross_sse(s, f);
	f = hvVectorNeg_sse(f);
	__m128 w = (__m128){0.0f, 0.0f, 0.0f, 1.0f};
	_MM_TRANSPOSE4_PS(s, u, f, w);
	for (unsigned int i = 0; i < 4; ++i)
		_mm_store_ps(
			&view->m[4*i],
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(s, _mm_set1_ps(T.m[4*i+0])) ,
					_mm_mul_ps(u, _mm_set1_ps(T.m[4*i+1])) ) ,
				_mm_add_ps(
					_mm_mul_ps(f, _mm_set1_ps(T.m[4*i+2])) ,
					_mm_mul_ps(w, _mm_set1_ps(T.m[4*i+3])) ) ) );
}

