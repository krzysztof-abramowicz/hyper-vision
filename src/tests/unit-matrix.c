/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Unit tests
 *     toolset: ISO C '99 + GAS + glib
 *     content: Test cases for the [matrix] module (text/x-c)
 *     version: 1.0
 */

#include "output.h"
#include "unit-matrix.h"

#include <glib.h>
#include <time.h>

#undef I

/*
 *    1. VECTOR OPERATIONS
 */

/* Input data for vector test cases */
static const vec3_t VECTOR_0 = VEC(0.0f, 0.0f, 0.0f);
static const vec3_t VECTOR_1 = VEC(1.0f, 1.0f, 1.0f);

static const vec3_t VECTOR_X = VEC(1.0f, 0.0f, 0.0f);
static const vec3_t VECTOR_Y = VEC(0.0f, 1.0f, 0.0f);
static const vec3_t VECTOR_Z = VEC(0.0f, 0.0f, 1.0f);
// para⃗∥e⃗l, perpe⃗⊥u⃗lar
static const vec3_t VECTOR_A = VEC(1.0f, 2.0f, 3.0f);
static const vec3_t VECTOR_E = VEC(2.0f, 4.0f, 6.0f);
static const vec3_t VECTOR_U = VEC(2.0f, 2.0f,-2.0f);

// Unicode kit: | | | | | (1,2,4,6-per em space)

/* Test case for vector equality checking */
static void vec_equ_tc() {
	// a⃗ = a⃗
	g_assert(hvVectorEqual(VECTOR_A, VECTOR_A));
	g_assert(hvVectorEqual(VECTOR_E, VECTOR_E));
	// a⃗ ≠ e⃗
	g_assert(!hvVectorEqual(VECTOR_A, VECTOR_E));
	g_assert(!hvVectorEqual(VECTOR_E, VECTOR_U));
}

/* Test case for vector negation, addition, substraction */
static void vec_nas_tc() {
	// -(-a⃗) = a⃗
	g_assert(hvVectorEqual(VECTOR_A, hvVectorNeg(hvVectorNeg(VECTOR_A))));
	// a⃗ ± 0⃗ = a⃗
	g_assert(hvVectorEqual(hvVectorAdd(VECTOR_A, VECTOR_0), VECTOR_A));
	g_assert(hvVectorEqual(hvVectorSub(VECTOR_A, VECTOR_0), VECTOR_A));
}

/* Test case for dot product */
static void vec_dot_tc() {
	// a⃗ ∙ 0⃗ = 0
	g_assert_cmpfloat(hvVectorDot(VECTOR_A, VECTOR_0), ==, 0.0f);
	// a⃗ ∙ e⃗ = e⃗ ∙ a⃗
	g_assert_cmpfloat(hvVectorDot(VECTOR_A, VECTOR_E), ==, hvVectorDot(VECTOR_E, VECTOR_A));
	// a⃗ ∙ (e⃗ + u⃗) = a⃗ ∙ e⃗ + a⃗ ∙ u⃗
	g_assert_cmpfloat(
		hvVectorDot(VECTOR_A, hvVectorAdd(VECTOR_E, VECTOR_U)), ==,
		hvVectorDot(VECTOR_A, VECTOR_E) + hvVectorDot(VECTOR_A, VECTOR_U)
	);
	// e⃗ ∙ u⃗ = 0  ⇔  e⃗ ⊥ u⃗
	g_assert_cmpfloat(hvVectorDot(VECTOR_E, VECTOR_U), ==, 0.0f);
	g_assert_cmpfloat(hvVectorDot(VECTOR_X, VECTOR_Y), ==, 0.0f);
	g_assert_cmpfloat(hvVectorDot(VECTOR_Y, VECTOR_Z), ==, 0.0f);
	g_assert_cmpfloat(hvVectorDot(VECTOR_Z, VECTOR_X), ==, 0.0f);
}

/* Test case for cross product */
static void vec_cross_tc() {
	// a⃗ × a⃗ = a⃗ × 0⃗ = 0⃗ × a⃗ = 0⃗ × 0⃗ = 0⃗
	g_assert(hvVectorEqual(hvVectorCross(VECTOR_A, VECTOR_A), VECTOR_0));
	g_assert(hvVectorEqual(hvVectorCross(VECTOR_A, VECTOR_0), VECTOR_0));
	g_assert(hvVectorEqual(hvVectorCross(VECTOR_0, VECTOR_A), VECTOR_0));
	g_assert(hvVectorEqual(hvVectorCross(VECTOR_0, VECTOR_0), VECTOR_0));
	// a⃗ × e⃗ = -e⃗ × a⃗
	g_assert(hvVectorEqual(
		hvVectorCross(VECTOR_A, VECTOR_E),
		hvVectorCross(hvVectorNeg(VECTOR_E), VECTOR_A)
	));
	// a⃗ × (e⃗ + u⃗) = a⃗ × e⃗ + a⃗ × u⃗
	g_assert(hvVectorEqual(
		hvVectorCross(VECTOR_A, hvVectorAdd(VECTOR_E, VECTOR_U)),
		hvVectorAdd(hvVectorCross(VECTOR_A, VECTOR_E), hvVectorCross(VECTOR_A, VECTOR_U))
	));
	// a⃗ × e⃗ = 0⃗  ⇔  a⃗ ∥ e⃗
	g_assert(hvVectorEqual(hvVectorCross(VECTOR_A, VECTOR_E), VECTOR_0));
}

/* Manual test for vector arithmetic (console output) */
static void vec_opr_print_tc() {
	#define V "\xE2\x83\x97"
	putchar('\n');
	hvLogInit(HV_LOG_INFO, stdout);
	hvLogTabWidth(4);
	hvPrint("\ttest vectors:");
	hvPrint("\t\t a"V" = ", hvVec(VECTOR_A));
	hvPrint("\t\t e"V" = ", hvVec(VECTOR_E));
	hvPrint("\t\t u"V" = ", hvVec(VECTOR_U));
	hvPrint(hvGray("\t\t para⃗∥e⃗l, perpe⃗⊥ u⃗lar"));
	hvPrint("\tmagnitude:");
	hvPrint("\t\t|a"V"| = ", hvFlt(hvVectorLen(VECTOR_A)));
	hvPrint("\t\t|e"V"| = ", hvFlt(hvVectorLen(VECTOR_E)));
	hvPrint("\t\t|u"V"| = ", hvFlt(hvVectorLen(VECTOR_U)));
	hvPrint("\tnegation:");
	hvPrint("\t\t-a"V" = ", hvVec(hvVectorNeg(VECTOR_A)));
	hvPrint("\t\t-e"V" = ", hvVec(hvVectorNeg(VECTOR_E)));
	hvPrint("\t\t-u"V" = ", hvVec(hvVectorNeg(VECTOR_U)));
	hvPrint("\tdot product:");
	hvPrint("\t\t a"V"∙a"V" = ", hvFlt(hvVectorDot(VECTOR_A, VECTOR_A)));	
	hvPrint("\t\t a"V"∙e"V" = ", hvFlt(hvVectorDot(VECTOR_A, VECTOR_E)));
	hvPrint("\t\t e"V"∙u"V" = ", hvFlt(hvVectorDot(VECTOR_E, VECTOR_U)));
	hvPrint("\tcross product:");
	hvPrint("\t\t a"V"×a"V" = ", hvVec(hvVectorCross(VECTOR_A, VECTOR_A)));
	hvPrint("\t\t a"V"×e"V" = ", hvVec(hvVectorCross(VECTOR_A, VECTOR_E)));
	hvPrint("\t\t e"V"×u"V" = ", hvVec(hvVectorCross(VECTOR_E, VECTOR_U)));	
	hvLogTerm();
	#undef V
}

/*
 *    2. MATRIX OPERATIONS
 */

/* Input data for matrix test cases */

static const mat4_t TEST_MATRIX_Z = {
	{ 0.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 0.0f }
};

static const mat4_t TEST_MATRIX_I = {
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f }
};

static const mat4_t TEST_MATRIX_A = {
	{ 1.0f, 4.0f, 7.0f, 0.0f,
	  2.0f, 5.0f, 8.0f, 0.0f,
	  3.0f, 6.0f, 9.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f }
};

static const mat4_t TEST_MATRIX_B = {
	{ 1.0f, 2.0f, 1.0f, 0.0f,
	  2.0f, 3.0f, 2.0f, 0.0f,
	  3.0f, 4.0f, 3.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f }
};

static const mat4_t TEST_MATRIX_AB = {
   { 08.0f, 20.0f, 32.0f, 0.0f,
     14.0f, 35.0f, 56.0f, 0.0f,
     20.0f, 50.0f, 80.0f, 0.0f,
     00.0f, 00.0f, 00.0f, 1.0f }
};

/* Test case for matrix equality checking */
static void mat_equ_tc() {
	// A = A
	g_assert(hvMatrixAreEqual(&TEST_MATRIX_A, &TEST_MATRIX_A));
	g_assert(hvMatrixAreEqual(&TEST_MATRIX_B, &TEST_MATRIX_B));
	// A ≠ B
	g_assert(!hvMatrixAreEqual(&TEST_MATRIX_A, &TEST_MATRIX_B));
	g_assert(!hvMatrixAreEqual(&TEST_MATRIX_B, &TEST_MATRIX_A));
}

/* Test case for matrix transposition */
static void mat_tra_tc() {
	// I^T = I
	mat4_t I = TEST_MATRIX_I;
	hvMatrixTranspose(&I);
	g_assert(hvMatrixAreEqual(&I, &TEST_MATRIX_I));
	// (A^T)^T = A
	mat4_t A = TEST_MATRIX_A;
	hvMatrixTranspose(&A);
	hvMatrixTranspose(&A);
	g_assert(hvMatrixAreEqual(&A, &TEST_MATRIX_A));
	// (B^T)^T = B
	mat4_t B = TEST_MATRIX_B;
	hvMatrixTranspose(&B);
	hvMatrixTranspose(&B);
	g_assert(hvMatrixAreEqual(&B, &TEST_MATRIX_B));
}

/* Test case for matrix multiplication */
static void mat_mul_test(const char* name, void (*multiply)(const mat4_t *, const mat4_t *, mat4_t *)) {
	mat4_t M;
	if (g_test_slow()) {
		printf("    %s: ", name);
		fflush(stdout);
	}
	// I × I = I
	multiply(&TEST_MATRIX_I, &TEST_MATRIX_I, &M);
	g_assert(hvMatrixAreEqual(&TEST_MATRIX_I, &M));
	// A × Z = Z
	multiply(&TEST_MATRIX_A, &TEST_MATRIX_Z, &M);
	g_assert(hvMatrixAreEqual(&TEST_MATRIX_Z, &M));
	// A × I = A
	multiply(&TEST_MATRIX_A, &TEST_MATRIX_I, &M);
	g_assert(hvMatrixAreEqual(&TEST_MATRIX_A, &M));
	// A × B = AB
	multiply(&TEST_MATRIX_A, &TEST_MATRIX_B, &M);
	g_assert(hvMatrixAreEqual(&TEST_MATRIX_AB, &M));
	// A^T × B^T = (BA)^T = AB
	mat4_t A = TEST_MATRIX_A, B = TEST_MATRIX_B;
	hvMatrixTranspose(&A);
	hvMatrixTranspose(&B);
	multiply(&B, &A, &M);
	hvMatrixTranspose(&M);
	g_assert(hvMatrixAreEqual(&TEST_MATRIX_AB, &M));
	if (g_test_slow()) puts("ok");
}
static void mat_mul_tc() {
	if (g_test_slow()) putchar('\n');
	mat_mul_test("current", hvMatrixMultiply);
	mat_mul_test("classic", hvMatrixMultiply_std);
	mat_mul_test("sse-int", hvMatrixMultiply_sse);
	mat_mul_test("sse-asm", hvMatrixMultiply_asm);
}

/* CPU warm-up procedure (precedes performance tests) */
static void mat_warmup_tc() {
	mat4_t viewMat;
	for (unsigned int i = 0; i < 5000000; ++i) {
		hvMatrixViewCamera_std(&viewMat, (vec3_s){{3, 0, 2}}, (vec3_s){{0, 0, 0}}, (vec3_s){{0, 1, 0}});
		hvMatrixViewCamera_sse(&viewMat, (__m128){3, 0, 2}, (__m128){0, 0, 0}, (__m128){0, 1, 0});
		hvMatrixViewCamera_std(&viewMat, (vec3_s){{2, 4, 8}}, (vec3_s){{-1, -2, -4}}, (vec3_s){{0, 0, 1}});
		hvMatrixViewCamera_sse(&viewMat, (__m128){2, 4, 8}, (__m128){-1, -2, -4}, (__m128){0, 0, 1});
	}
	mat4_t C;
	for (unsigned int i = 0; i < 5000000; ++i) {
		hvMatrixMultiply_std(&TEST_MATRIX_A, &TEST_MATRIX_B, &C);
		hvMatrixMultiply_sse(&TEST_MATRIX_B, &TEST_MATRIX_A, &C);
		hvMatrixMultiply_asm(&TEST_MATRIX_A, &TEST_MATRIX_B, &C);
	}
}

/* Performance test for matrix multiplication */
static double mat_mul_perf(const char* name, void (*multiply)(const mat4_t *, const mat4_t *, mat4_t *), double ref) {
	mat4_t C;
	double cur, min = 999.0f;
	for (unsigned int i = 0; i < 3; ++i) {
		clock_t t = clock();
		for (unsigned int i = 0; i < 2000000; ++i) {
			multiply(&TEST_MATRIX_A, &TEST_MATRIX_B, &C);
			multiply(&TEST_MATRIX_B, &TEST_MATRIX_A, &C);
		}
		cur = (double)(clock() - t) / CLOCKS_PER_SEC;
		if (cur < min) min = cur;
	}
	if (ref == 0.0f) {
		printf("    %s: %6.4f sec\n", name, min);
		g_test_minimized_result(min, "%s: %6.4f sec", name, min);
	} else {
		printf("    %s: %6.4f sec (%d%%)\n", name, min, (int)(min/ref * 100.0f));
		g_test_minimized_result(min, "%s: %6.4f sec (%d%%)", name, min, (int)(min/ref * 100.0f));
	}
	return min;
}
static void mat_mul_perf_tc() {
	printf("\n  4M matrix multiplications in:\n");
	double ref = 
	mat_mul_perf("classic", hvMatrixMultiply_std, 0.0f);
	mat_mul_perf("sse-asm", hvMatrixMultiply_asm, ref);
	mat_mul_perf("sse-int", hvMatrixMultiply_sse, ref);	
}

/* Performance test for Rotation Matrix calculation */
static double mat_rot_perf(const char* name, void (*rotate)(mat4_t *, float), double ref) {
	mat4_t R;
	double cur, min = 999.0f;
	for (unsigned int i = 0; i < 3; ++i) {
		clock_t t = clock();
		hvMatrixIdentity_sse(&R);
		for (unsigned int i = 0; i < 1000000; ++i) {
			rotate(&R, 45.0f);
			rotate(&R, 60.0f);
		}
		cur = (double)(clock() - t) / CLOCKS_PER_SEC;
		if (cur < min) min = cur;
	}
	if (ref == 0.0f) {
		printf("    %s: %6.4f sec\n", name, min);
		g_test_minimized_result(min, "%s: %6.4f sec", name, min);
	} else {
		printf("    %s: %6.4f sec (%d%%)\n", name, min, (int)(min/ref * 100.0f));
		g_test_minimized_result(min, "%s: %6.4f sec (%d%%)", name, min, (int)(min/ref * 100.0f));
	}
	return min;
}
static void mat_rot_perf_tc() {
	printf("\n  2M matrix rotations in:\n");
	double ref = 
	mat_rot_perf("std-clib", hvMatrixRotateX_std_clib, 0.0f);
	mat_rot_perf("std-ltab", hvMatrixRotateX_std_ltab, ref);
	mat_rot_perf("sse-clib", hvMatrixRotateX_sse_clib, ref);
	mat_rot_perf("sse-ltab", hvMatrixRotateX_sse_ltab, ref);
	mat_rot_perf("sse-expv", hvMatrixRotateX_sse_ltab_iv, ref);
}

/* Performance test for View Matrix calculation */
static void mat_view_perf_tc() {
	mat4_t viewMat;
	printf("\n  2M view matrix calculations in:\n");
	double cur;
	// standard computation
	double min_std = 999.0f;
	for (unsigned int i = 0; i < 3; ++i) {
		clock_t t = clock();
		for (unsigned int i = 0; i < 1000000; ++i) {
			hvMatrixViewCamera_std(&viewMat, (vec3_s){{3, 0, 2}}, (vec3_s){{0, 0, 0}}, (vec3_s){{0, 1, 0}});
			hvMatrixViewCamera_std(&viewMat, (vec3_s){{2, 4, 8}}, (vec3_s){{-1, -2, -4}}, (vec3_s){{0, 0, 1}});
		}
		cur = (double)(clock() - t) / CLOCKS_PER_SEC;
		if (cur < min_std) min_std = cur;
	}
	printf("    scalar: %6.4f sec\n", min_std);
	g_test_minimized_result(min_std, "scalar: %6.4f sec", min_std);
	// SSE computation
	double min_sse = 999.0f;
	for (unsigned int i = 0; i < 3; ++i) {
		clock_t t = clock();
		for (unsigned int i = 0; i < 1000000; ++i) {
			hvMatrixViewCamera_sse(&viewMat, (__m128){3, 0, 2}, (__m128){0, 0, 0}, (__m128){0, 1, 0});
			hvMatrixViewCamera_sse(&viewMat, (__m128){2, 4, 8}, (__m128){-1, -2, -4}, (__m128){0, 0, 1});
		}
		cur = (double)(clock() - t) / CLOCKS_PER_SEC;
		if (cur < min_sse) min_sse = cur;
	}
	printf("    vector: %6.4f sec (%d%%)\n", min_sse, (int)(min_sse/min_std * 100.0f));
	g_test_minimized_result(min_sse, "vector: %6.4f sec (%d%%)", min_sse, (int)(min_sse/min_std * 100.0f));
}

/* Manual test for matrix multiplication (console output) */
static void mat_mul_print(const char* name, void (*multiply)(const mat4_t *, const mat4_t *, mat4_t *)) {
	printf("    %s matrix multiplication:\n", name);
	hvFrameInit(92, 6);
	hvFrameColumnPadding(1);
	hvFrameColumnNext();
	hvFrameColumnNext();
	hvFrameColumnNext();
	hvFrameColumnNext();
		mat4_t A = TEST_MATRIX_A;
		hvMatrixPrintToFrameI(&A);
	hvFrameRowSkip(3);
	hvFrameRowPuts("x");
	hvFrameRowSkip(2);
		mat4_t B = TEST_MATRIX_B;
		hvMatrixPrintToFrameI(&B);
	hvFrameRowSkip(3);
	hvFrameRowPuts("=");
	hvFrameRowSkip(2);
		mat4_t C = TEST_MATRIX_Z;
		multiply(&A, &B, &C);
		hvMatrixPrintToFrameI(&C);	
	hvFramePrint();
}
static void mat_mul_print_tc() {
	putchar('\n');
	mat_mul_print("current", hvMatrixMultiply);
	mat_mul_print("classic", hvMatrixMultiply_std);
	mat_mul_print("SSE intrinsics", hvMatrixMultiply_sse);	
	mat_mul_print("x86-64 assembly", hvMatrixMultiply_asm);
}

/* Manual test for view matrix calculation (console output) */
static void mat_view_print_tc() {
	mat4_t V;
	printf("\n     standard view matrices calculation:\n");
	hvFrameInit(92, 6);
	hvFrameColumnPadding(1);
	hvFrameColumnNext();
	hvFrameColumnNext();
	hvFrameColumnNext();
	hvFrameColumnNext();
	hvMatrixViewCamera_std(&V, (vec3_s){{3, 0, 2}}, (vec3_s){{0, 0, 0}}, (vec3_s){{0, 1, 0}});
	hvMatrixPrintToFrameI(&V);
	hvFrameColumnNext();
	hvMatrixViewCamera_std(&V, (vec3_s){{2, 4, 8}}, (vec3_s){{-1, -2, -4}}, (vec3_s){{0, 0, 1}});
	hvMatrixPrintToFrameI(&V);
	hvFramePrint();
	printf("\n     SSE view matrices calculation:\n");
	hvFrameInit(92, 6);
	hvFrameColumnPadding(1);
	hvFrameColumnNext();
	hvFrameColumnNext();
	hvFrameColumnNext();
	hvFrameColumnNext();
	hvMatrixViewCamera_sse(&V, (__m128){3, 0, 2}, (__m128){0, 0, 0}, (__m128){0, 1, 0});
	hvMatrixPrintToFrameI(&V);
	hvFrameColumnNext();
	hvMatrixViewCamera_sse(&V, (__m128){2, 4, 8}, (__m128){-1, -2, -4}, (__m128){0, 0, 1});
	hvMatrixPrintToFrameI(&V);
	hvFramePrint();
}

/*
 *    Unit test execution block
 */

int main (int argc, char *argv[]) {
	g_test_init(&argc, &argv, NULL);
	// Vector test cases
	g_test_add_func("/hv/vector/compare", vec_equ_tc);
	g_test_add_func("/hv/vector/neg-add-sum", vec_nas_tc);
	g_test_add_func("/hv/vector/dot-product", vec_dot_tc);
	g_test_add_func("/hv/vector/cross-product", vec_cross_tc);
	// Matrix test cases
	g_test_add_func("/hv/matrix/compare", mat_equ_tc);
	g_test_add_func("/hv/matrix/transpose", mat_tra_tc);
	g_test_add_func("/hv/matrix/multiply", mat_mul_tc);
	// Performance tests
	if (g_test_perf()) {
		g_test_add_func("/hv/matrix/warm-up", mat_warmup_tc);
		g_test_add_func("/hv/matrix/multiply-perf", mat_mul_perf_tc);
		g_test_add_func("/hv/matrix/rotate-perf", mat_rot_perf_tc);
		g_test_add_func("/hv/matrix/view-perf", mat_view_perf_tc);
	}
	// Manual tests
	if (g_test_slow()) {
		g_test_add_func("/hv/vector/operation-print", vec_opr_print_tc);
		g_test_add_func("/hv/matrix/multiply-print", mat_mul_print_tc);
		g_test_add_func("/hv/matrix/view-print", mat_view_print_tc);
	}
	// Ready, steady, go!
	return g_test_run();
}

