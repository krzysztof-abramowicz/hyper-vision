
#include "matrix.h"
#include "matrix_p.h"

//: General-purpose vector arithmetic

extern inline _Bool hvVectorEqual(vec3_t a, vec3_t b) {
	#if HV_OPERATION_MODE == HV_SCALAR
		return
			a.v[0] == b.v[0] &&
			a.v[1] == b.v[1] &&
			a.v[2] == b.v[2] ;
	#elif HV_OPERATION_MODE == HV_VECTOR
		return _mm_movemask_ps( _mm_cmpeq_ps(a, b) ) == 0xf;
	#endif
}

extern inline vec3_t hvVectorNeg(vec3_t a) {
	#if HV_OPERATION_MODE == HV_SCALAR
		return (vec3_t) {{
			-a.v[0],
			-a.v[1],
			-a.v[2]
		}};
	#elif HV_OPERATION_MODE == HV_VECTOR
		return _mm_xor_ps(a, _mm_castsi128_ps(
			_mm_set1_epi32(1U << 31)
		));
	#endif
}

extern inline vec3_t hvVectorAdd(vec3_t a, vec3_t b) {
	#if HV_OPERATION_MODE == HV_SCALAR
		return (vec3_t) {{
			a.v[0] + b.v[0],
			a.v[1] + b.v[1],
			a.v[2] + b.v[2]
		}};
	#elif HV_OPERATION_MODE == HV_VECTOR
		return _mm_add_ps(a, b);
	#endif
}

extern inline vec3_t hvVectorSub(vec3_t a, vec3_t b) {
	#if HV_OPERATION_MODE == HV_SCALAR
		return (vec3_t) {{
			a.v[0] - b.v[0],
			a.v[1] - b.v[1],
			a.v[2] - b.v[2]
		}};
	#elif HV_OPERATION_MODE == HV_VECTOR
		return _mm_sub_ps(a, b);
	#endif
}

extern inline float hvVectorDot(vec3_t a, vec3_t b) {
	#if HV_OPERATION_MODE == HV_SCALAR
		return
			a.v[0] * b.v[0] +
			a.v[1] * b.v[1] +
			a.v[2] * b.v[2] ;
	#elif HV_OPERATION_MODE == HV_VECTOR
		a = _mm_mul_ps(a, b);
		a = _mm_add_ps(a, _mm_shuffle_ps(a, a, 0xB1));
		a = _mm_add_ps(a, _mm_shuffle_ps(a, a, 0x1B));
		return _mm_cvtss_f32(a);
	#endif
}

extern inline float hvVectorLen(vec3_t vec) {
	#if HV_OPERATION_MODE == HV_SCALAR
		return sqrt(
			vec.v[0] * vec.v[0] +
			vec.v[1] * vec.v[1] +
			vec.v[2] * vec.v[2]
		);
	#elif HV_OPERATION_MODE == HV_VECTOR
		vec = _mm_mul_ps(vec, vec);
		vec = _mm_add_ps(vec, _mm_shuffle_ps(vec, vec, 0xB1));
		vec = _mm_add_ps(vec, _mm_shuffle_ps(vec, vec, 0x1B));
		vec = _mm_sqrt_ss(vec);
		return _mm_cvtss_f32(vec);
	#endif
}

extern inline vec3_t hvVectorCross(vec3_t a, vec3_t b) {
	#if HV_OPERATION_MODE == HV_SCALAR
		return (vec3_t) {{
			a.v[Y] * b.v[Z] - a.v[Z] * b.v[Y],
			a.v[Z] * b.v[X] - a.v[X] * b.v[Z],
			a.v[X] * b.v[Y] - a.v[Y] * b.v[X]
		}};
	#elif HV_OPERATION_MODE == HV_VECTOR
		__m128 res =
			_mm_sub_ps(
				_mm_mul_ps(a, _mm_shuffle_ps(b, b, 0xC9)),
				_mm_mul_ps(b, _mm_shuffle_ps(a, a, 0xC9))
			);
		return _mm_shuffle_ps(res, res, 0xC9);
	#endif
}

extern inline vec3_t hvVectorNorm(vec3_t vec) {
	float len = hvVectorLen(vec);
	#if HV_OPERATION_MODE == HV_SCALAR
		if (len != 0.0f)
			return (vec3_t) {{
				vec.v[0] / len,
				vec.v[1] / len,
				vec.v[2] / len
			}};
		else
			return (vec3_t){{0.0f, 0.0f, 0.0f}};
	#elif HV_OPERATION_MODE == HV_VECTOR
		if ( __builtin_expect(len != 0.0f, 1) )
			return _mm_div_ps(vec, _mm_set1_ps(len)); //? multiply by reciprocal
		else
			return (vec3_t){0.0f, 0.0f, 0.0f, 0.0f};
	#endif
}

float hvVectorAngle(vec3_t a, vec3_t b) {	
	return acos(
		 hvVectorDot(a, b) /
		(hvVectorLen(a) * hvVectorLen(b))
	);
}

#if HV_OPERATION_MODE == HV_VECTOR
static inline vec3_t hvVectorPos(vec3_t vec) {
		static const float one = 1.0f;
		__m128 tmp = _mm_move_ss(vec, _mm_load_ss(&one));
		return _mm_shuffle_ps(vec, tmp, _MM_SHUFFLE(0,2,1,0));
}
#endif

//: General-purpose matrix arithmetic

extern inline void hvMatrixZero(mat4_t * mat) {
#if HV_OPERATION_MODE == HV_SCALAR
	memset(mat->m, 0, 16 * sizeof(float));
#elif HV_OPERATION_MODE == HV_VECTOR
	__m128 zero = __mm_setzero_ps();
	_mm_store_ps(&mat->m[ 0], zero);
	_mm_store_ps(&mat->m[ 4], zero);
	_mm_store_ps(&mat->m[ 8], zero);
	_mm_store_ps(&mat->m[12], zero);
#endif
}

extern inline void hvMatrixIdentity(mat4_t * mat) {
#if HV_OPERATION_MODE == HV_SCALAR
	memcpy(mat->m, HV_IDENTITY_MATRIX.m, 16 * sizeof(float));
#elif HV_OPERATION_MODE == HV_VECTOR
	_mm_store_ps(&mat->m[ 0], _mm_load_ps(&HV_IDENTITY_MATRIX.m[ 0]));
	_mm_store_ps(&mat->m[ 4], _mm_load_ps(&HV_IDENTITY_MATRIX.m[ 4]));
	_mm_store_ps(&mat->m[ 8], _mm_load_ps(&HV_IDENTITY_MATRIX.m[ 8]));
	_mm_store_ps(&mat->m[12], _mm_load_ps(&HV_IDENTITY_MATRIX.m[12]));
#endif
}

_Bool hvMatrixAreEqual(const mat4_t * restrict mat_a, const mat4_t * restrict mat_b) {
	for (unsigned int i = 0; i < 16; ++i)
		if (mat_a->m[i] != mat_b->m[i])
			return false;
	return true;
}

void hvMatrixTranspose(mat4_t * restrict mat) {
#if HV_OPERATION_MODE == HV_SCALAR
	float tmp;
	tmp = mat->m[ 1];  mat->m[ 1] = mat->m[ 4];  mat->m[ 4] = tmp;
	tmp = mat->m[ 2];  mat->m[ 2] = mat->m[ 8];  mat->m[ 8] = tmp;
	tmp = mat->m[ 3];  mat->m[ 3] = mat->m[12];  mat->m[12] = tmp;
	tmp = mat->m[ 6];  mat->m[ 6] = mat->m[ 9];  mat->m[ 9] = tmp;
	tmp = mat->m[ 7];  mat->m[ 7] = mat->m[13];  mat->m[13] = tmp;
	tmp = mat->m[11];  mat->m[11] = mat->m[14];  mat->m[14] = tmp;
#elif HV_OPERATION_MODE == HV_VECTOR
	__m128 col0 = _mm_load_ps(&mat->m[Xx]);
	__m128 col1 = _mm_load_ps(&mat->m[Yx]);
	__m128 col2 = _mm_load_ps(&mat->m[Zx]);
	__m128 col3 = _mm_load_ps(&mat->m[Tx]);
	_MM_TRANSPOSE4_PS(col0, col1, col2, col3);
	_mm_store_ps(&mat->m[Xx], col0);
	_mm_store_ps(&mat->m[Yx], col1);
	_mm_store_ps(&mat->m[Zx], col2);
	_mm_store_ps(&mat->m[Tx], col3);
#endif
}

void hvMatrixMultiply(const mat4_t *A, const mat4_t *B, mat4_t *R) {
	#if HV_OPERATION_MODE == HV_SCALAR
		for (unsigned int i = 0; i < 16; i += 4)
			for (unsigned int j = 0; j < 4; ++j)
				R->m[i + j] = (B->m[i + 0] * A->m[j +  0])
					        + (B->m[i + 1] * A->m[j +  4])
					        + (B->m[i + 2] * A->m[j +  8])
					        + (B->m[i + 3] * A->m[j + 12]);
	#elif HV_OPERATION_MODE == HV_VECTOR
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
			_mm_store_ps(&R->m[4*i], row);
		}
	#endif
}

#if HV_OPERATION_MODE == HV_VECTOR
static inline void hvMatrixMultiplyI(const mat4_t *A, mat4_t *B) {
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
#endif

//: Affine Matrix transformations
//. When applied to a model, map its vertices from Model Space to World Space,
//. thus placing it on the scene.

void hvMatrixResize(mat4_t * restrict mat, float factor) {
	mat4_t scale;
	hvMatrixIdentity(&scale);
	scale.m[Sx] = factor;
	scale.m[Sy] = factor;
	scale.m[Sz] = factor;
	#if HV_OPERATION_MODE == HV_SCALAR
		mat4_t tmp_mat;	
		hvMatrixMultiply(&scale, mat, &tmp_mat);
		memcpy(mat->m, &tmp_mat, sizeof(mat4_t));
	#elif HV_OPERATION_MODE == HV_VECTOR
		hvMatrixMultiplyI(&scale, mat);
	#endif
}

void hvMatrixScale(mat4_t * restrict mat, float xFact, float yFact, float zFact) {
	mat4_t scale;
	hvMatrixIdentity(&scale);
	scale.m[Sx] = xFact;
	scale.m[Sy] = yFact;
	scale.m[Sz] = zFact;
	#if HV_OPERATION_MODE == HV_SCALAR
		mat4_t tmp_mat;	
		hvMatrixMultiply(&scale, mat, &tmp_mat);
		memcpy(mat->m, &tmp_mat, sizeof(mat4_t));
	#elif HV_OPERATION_MODE == HV_VECTOR
		hvMatrixMultiplyI(&scale, mat);
	#endif
}

void hvMatrixRotateX(mat4_t * restrict mat, float angle) {
	mat4_t rotate;
	hvMatrixIdentity(&rotate);
	float sine   = (float) hvSin( hvAngle(angle) );
	float cosine = (float) hvCos( hvAngle(angle) );
	rotate.m[Yy] =  cosine;
	rotate.m[Yz] =  sine;
	rotate.m[Zy] = -sine;
	rotate.m[Zz] =  cosine;
	#if HV_OPERATION_MODE == HV_SCALAR
		mat4_t tmp_mat;	
		hvMatrixMultiply(&rotate, mat, &tmp_mat);
		memcpy(mat->m, &tmp_mat, sizeof(mat4_t));
	#elif HV_OPERATION_MODE == HV_VECTOR
		hvMatrixMultiplyI(&rotate, mat);
	#endif
}

void hvMatrixRotateY(mat4_t * restrict mat, float angle) {
	mat4_t rotate;
	hvMatrixIdentity(&rotate);
	float sine   = (float) hvSin( hvAngle(angle) );
	float cosine = (float) hvCos( hvAngle(angle) );
	rotate.m[Xx] =  cosine;
	rotate.m[Xz] = -sine;
	rotate.m[Zx] =  sine;
	rotate.m[Zz] =  cosine;
	#if HV_OPERATION_MODE == HV_SCALAR
		mat4_t tmp_mat;	
		hvMatrixMultiply(&rotate, mat, &tmp_mat);
		memcpy(mat->m, &tmp_mat, sizeof(mat4_t));
	#elif HV_OPERATION_MODE == HV_VECTOR
		hvMatrixMultiplyI(&rotate, mat);
	#endif
}

void hvMatrixRotateZ(mat4_t * restrict mat, float angle) {
	mat4_t rotate;
	hvMatrixIdentity(&rotate);
	float sine   = (float) hvSin( hvAngle(angle) );
	float cosine = (float) hvCos( hvAngle(angle) );
	rotate.m[Xx] =  cosine;
	rotate.m[Xy] =  sine;
	rotate.m[Yx] = -sine;
	rotate.m[Yy] =  cosine;
	#if HV_OPERATION_MODE == HV_SCALAR
		mat4_t tmp_mat;	
		hvMatrixMultiply(&rotate, mat, &tmp_mat);
		memcpy(mat->m, &tmp_mat, sizeof(mat4_t));
	#elif HV_OPERATION_MODE == HV_VECTOR
		hvMatrixMultiplyI(&rotate, mat);
	#endif
}

void hvMatrixTranslate(mat4_t * restrict mat, vec3_t dir) {
#if HV_OPERATION_MODE == HV_SCALAR
	mat4_t trans, tmp_mat;
	hvMatrixIdentity(&trans);
	trans.m[Tx] = dir.v[X];
	trans.m[Ty] = dir.v[Y];
	trans.m[Tz] = dir.v[Z];
	hvMatrixMultiply(&trans, mat, &tmp_mat);
	memcpy(mat->m, &tmp_mat, sizeof(mat4_t));
#elif HV_OPERATION_MODE == HV_VECTOR
	mat4_t trans;
	hvMatrixIdentity(&trans);
	_mm_store_ps(&trans.m[Tx], dir);
	trans.m[Tw] = 1.0f;
	hvMatrixMultiplyI(&trans, mat);
#endif
}

//: View Matrix creation
//. View Matrices created with the following functions map vertices from
//.	World Space to View Space (keeping coordinate system's handedness).
//> http://www.opengl.org/sdk/docs/man2/

void hvMatrixViewCamera(mat4_t * restrict view, vec3_t cam, vec3_t ref, vec3_t up) {
#if HV_OPERATION_MODE == HV_SCALAR
	// build translation matrix
	mat4_t trans;
	hvMatrixIdentity(&trans);
	trans.m[Tx] = -cam.v[X];
	trans.m[Ty] = -cam.v[Y];
	trans.m[Tz] = -cam.v[Z];
	// compute camera's coordinate system vectors
	vec3_t u = hvVectorNorm(up);
	vec3_t f = hvVectorNorm(hvVectorSub(ref, cam));
	vec3_t s = hvVectorNorm(hvVectorCross(f, u));
	u = hvVectorCross(s, f);
	// build rotation matrix
	mat4_t rotat;
	hvMatrixIdentity(&rotat);
	rotat.m[Xx] =  s.v[X];
	rotat.m[Yx] =  s.v[Y];
	rotat.m[Zx] =  s.v[Z];
	rotat.m[Xy] =  u.v[X];
	rotat.m[Yy] =  u.v[Y];
	rotat.m[Zy] =  u.v[Z];
	rotat.m[Xz] = -f.v[X];
	rotat.m[Yz] = -f.v[Y];
	rotat.m[Zz] = -f.v[Z];
	// combine the matrices
	hvMatrixMultiply(&rotat, &trans, view);
#elif HV_OPERATION_MODE == HV_VECTOR
	// build translation matrix (on stack)
	mat4_t T;
	hvMatrixIdentity(&T);
	_mm_store_ps(&T.m[Tx], hvVectorNeg(cam));
	T.m[Tw] = 1.0f;
	// compute camera's coordinate system vectors
	vec3_t f = hvVectorNorm(hvVectorSub(ref, cam));
	vec3_t s = hvVectorNorm(hvVectorCross(f, up));	
	vec3_t u = hvVectorCross(s, f);
	// build rotation matrix (in registers)
	f = hvVectorNeg(f);
	vec3_t w = (__m128){0.0f, 0.0f, 0.0f, 1.0f};
	_MM_TRANSPOSE4_PS(s, u, f, w);
	// combine the matrices
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
#endif
}

//: Projective matrix transformations
//! Transformations defined below flip the Z axis, thus changing
//! the coordinate system from right-handed (OpenGL) to left-handed (hardware)
//> http://www.opengl.org/sdk/docs/man2/
//> http://antongerdelan.net/opengl/

void hvMatrixProjectionCubic(mat4_t *proj, float edge) {
	hvMatrixZero(proj);
	proj->m[Sx] =  1.0f / edge;
	proj->m[Sy] =  1.0f / edge;
	proj->m[Sz] = -1.0f / edge; //!
}

void hvMatrixProjectionPlain(mat4_t *proj, float left, float right, float bottom, float top) {
	hvMatrixZero(proj);
	proj->m[Sx] =  2.0f / (right - left);
	proj->m[Sy] =  2.0f / (top - bottom);
	proj->m[Sz] = -1.0f; //!
	proj->m[Tx] = -(right + left) / (right - left);
	proj->m[Ty] = -(top + bottom) / (top - bottom);
}

void hvMatrixProjectionOrthographic(mat4_t *proj, float left, float right, float bottom, float top, float near, float far) {
	hvMatrixZero(proj);
	proj->m[Sx] =  2.0f / (right - left);
	proj->m[Sy] =  2.0f / (top - bottom);
	proj->m[Sz] = -2.0f / (far - near); //!
	proj->m[Tx] = -(right + left) / (right - left);
	proj->m[Ty] = -(top + bottom) / (top - bottom);
	proj->m[Tz] = -(far + near) / (far - near);
}

void hvMatrixProjectionFrustum(mat4_t *proj, float left, float right, float bottom, float top, float near, float far) {
	hvMatrixZero(proj);
	proj->m[Sx] =  (2.0f * near) / (right - left);
	proj->m[Sy] =  (2.0f * near) / (top - bottom);
	proj->m[Zx] =  (right + left) / (right - left);
	proj->m[Zy] =  (top + bottom) / (top - bottom);
	proj->m[Zz] = -(far + near) / (far - near);
	proj->m[Zw] = -1; //!
	proj->m[Tz] = -(2.0f * far * near) / (far - near);
}

void hvMatrixProjectionPerspective(mat4_t *proj, float yFOV, float aspect, float near, float far) {
	hvMatrixZero(proj);
	float func = hvCtg( hvAngle(yFOV / 2.0f) );
	proj->m[Sx] =  func / aspect;
	proj->m[Sy] =  func;
	proj->m[Sz] =  (far + near) / (near - far);
	proj->m[Zw] = -1; //!
	proj->m[Tz] =  (2.0f * far * near) / (near - far);
}

void hvMatrixProjectionPerspectiveAG(mat4_t *proj, float yFOV, float aspect, float near, float far) {
	hvMatrixZero(proj);
	float range = hvCtg(hvAngle(yFOV * .5f)) * near;
	proj->m[Sx] =  (2.0f * near) / (range * aspect + range * aspect); 
	proj->m[Sy] =  near / range;
	proj->m[Sz] = -(far + near) / (far - near);
	proj->m[Zw] = -1; //!
	proj->m[Tz] = -(2.0f * far * near) / (far - near);
}

//: Basic matrix-related printing functions

void hvMatrixPrintF(const mat4_t *mat) {
	printf("\t┌─                           ─┐\n");
	for (unsigned int i = 0; i < 16; i += 4)
		printf("\t│ %5.2f  %5.2f  %5.2f  %5.2f  │\n", mat->m[i], mat->m[i+1], mat->m[i+2], mat->m[i+3]);
	printf("\t└─                           ─┘\n");
}

void hvMatrixPrintI(const mat4_t *mat) {
	printf("┌─               ─┐\n");
	for (unsigned int i = 0; i < 16; i += 4)
		printf("│%3.0f %3.0f %3.0f %3.0f  │\n", mat->m[i], mat->m[i+1], mat->m[i+2], mat->m[i+3]);
	printf("└─               ─┘\n");
}

//: Advanced matrix-related printing functions
//! Imply dependence on the [output] module

#ifdef HV_DS1_OUTPUT_H

	static char vec_buf[HV_SIZE_VEC]; //!

	const char * hvVec(vec3_t vec) {
		#if HV_OPERATION_MODE == HV_SCALAR
			return hvLogString(7, "[", hvFlt(vec.v[X]), ", ", hvFlt(vec.v[Y]), ", ", hvFlt(vec.v[Z]), "]");
		#elif HV_OPERATION_MODE == HV_VECTOR
			return hvLogString(7, "[", hvFlt(vec[X]), ", ", hvFlt(vec[Y]), ", ", hvFlt(vec[Z]), "]");
		#endif
	}

	const char* hvVec3(vec3_t vec) {
		#if HV_OPERATION_MODE == HV_SCALAR
			snprintf(vec_buf, HV_SIZE_VEC, "[%5.2f, %5.2f, %5.2f]", vec.v[X], vec.v[Y], vec.v[Z]);
			return vec_buf;
		#elif HV_OPERATION_MODE == HV_VECTOR
			snprintf(vec_buf, HV_SIZE_VEC, "[%5.2f, %5.2f, %5.2f]", vec[X], vec[Y], vec[Z]);
			return vec_buf;
		#endif
	}

	void hvMatrixPrintFC(const mat4_t *mat) {
		hvPrint("\t┌─                      ─┐");
		hvPrint("\t│", hvRed(   hvFlt(mat->m[Xx]), hvFlt(mat->m[Xy]), hvFlt(mat->m[Xz]) ), hvFlt(mat->m[Xw]), "│");
		hvPrint("\t│", hvGreen( hvFlt(mat->m[Yx]), hvFlt(mat->m[Yy]), hvFlt(mat->m[Yz]) ), hvFlt(mat->m[Yw]), "│");
		hvPrint("\t│", hvBlue(  hvFlt(mat->m[Zx]), hvFlt(mat->m[Zy]), hvFlt(mat->m[Zz]) ), hvFlt(mat->m[Zw]), "│");
		hvPrint("\t│", hvGray(  hvFlt(mat->m[Tx]), hvFlt(mat->m[Ty]), hvFlt(mat->m[Tz]) ), hvFlt(mat->m[Tw]), "│");
		hvPrint("\t└─                      ─┘");
	}

	void hvMatrixPrintToFrameI(const mat4_t *mat) {
		char* tmp_str = (char*) malloc(64);
		hvFrameRowPuts("+-               -+");
		for (unsigned int i = 0; i < 4; ++i) {
			sprintf(tmp_str, "|%3.0f %3.0f %3.0f %3.0f  |", mat->m[i], mat->m[i+4], mat->m[i+8], mat->m[i+12]);
			hvFrameRowPuts(tmp_str);
		}
		hvFrameRowPuts("+-               -+");
	}

	void hvMatrixPrintToFrameF(const mat4_t *mat) {
		char* tmp_str = (char*) malloc(64);
		hvFrameRowPuts("┌─                           -+");
		for (unsigned int i = 0; i < 4; ++i) {
			sprintf(tmp_str, "│ %5.2f  %5.2f  %5.2f  %5.2f  │", mat->m[i], mat->m[i+4], mat->m[i+8], mat->m[i+12]);
			hvFrameRowPuts(tmp_str);
		}
		hvFrameRowPuts("+-                           -+\n");
	}

#endif

