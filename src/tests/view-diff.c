/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─View tests
 *     toolset: ISO C'99 + ISO C++'11
 *     content: View on differences in common features of C'99 and C++'11
 *     version: 1.0
 *
 *
 * This file compiles in two modes:
 *   • ISO C++'11 (label `C++`, macro `HV_CPP`)
 *   • ISO C'99 (label `C`, macro `HV_C`)
 *
 * As of ISO C'99, a few C++ features are available in C, these include:
 *   • line comments (`// comment`)
 *   • local variables' definition within blocks and statements (`for (int i=0, i<10, ++i)`)
 */

#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

#if __cplusplus
	#include <cstdio>
	#include <vector>
#else
	#include <stdio.h>
#endif

#define HV_CPP 2
#define HV_C 1

#if __cplusplus
	namespace {
		void static_func();
	}
#else
	static void static_func(void);
#endif

/* 1. Function call conformance to decaration and definition
 *    C++ : a function call must stricly conform to its declaration (and definition)
 *    C   : a function call may include not declared arguments (defined or not)
 */

// the `int abs(int)` function is formally declared in stdib.h
#if __cplusplus
	extern "C" int abs(int);
#else
	int abs();
#endif

void args_tc() {
	#if __cplusplus
		printf("abs: %d\n", abs(-5));    // decl_argc = impl_argc = call_argc
	#else
		printf("abs: %d\n", abs(-5));    // decl_argc < impl_argc = call_argc
		printf("abs: %d\n", abs(-5, 7)); // decl_argc < impl_argc < call_argc
	#endif
}

/* 2. Interpretation of single-quoted character (eg., `'c'`)
 *    C++ : A single-quoted character is a character constant, thus `sizeof('c')` equals 1
 *    C   : A single-quoted character is an integer constant, thus `sizeof('c')` equals 4
 */
void sizeof_tc() {
	printf("size of 'c': %zu\n", sizeof('c'));
}

/* 3. Definition of NULL constant and nullptr keyword
 *    C++ : The nullptr keyword should be used for null pointers, NULL is defined as `0`
 *    C   : The NULL macro is usually used for null pointers, it is defined as `((void *)0)`
 */
void nullptr_tc() {
	#if __cplusplus
		int *pointer = nullptr;
	#else
		int *pointer = NULL;
	#endif
}

/* 4. The `void` parameter list
 *    C++ : Declaration with empty parameter list implies no parameters
 *    C   : Empty list does not specify parameters, `void` implies no parameters 
 */
#if __cplusplus
	void voidarg();    // declaration
	void voidarg() {   // definition
		printf("a nullary function in C++'11\n");
	}
#else
	void voidarg();       // partial declaration (unspecified parameters)
	void voidarg(void);   // declaration (no parameters)
	void voidarg(void) {  // definition
		printf("a nullary function in C'99\n");
	}	
#endif

void voidarg_tc() {
	voidarg();
}

/* 5. Compound type declarations
 *    C++ : The compound types' identifiers appear in current namespace as type specifiers
 *    C   : There are separate "namespces" for `union`, `enum` and `struct` types. These keywords
 *        : must always precede the type identifiers unless `typedef` is used to "promote" them.
 */
void compound_tc() {
	#if HV_LANG == HV_CPP
		struct Point {
			int x;
			int y;
		};
		Point p;
		p.x = 5;
		p.x++;
	#else
		// struct definition and use
		struct point {
			int x;
			int y;
		};
		struct point p1;
		p1.x = 5;
		p1.x++;
		// defining struct as a type
		typedef struct point point_t;
		point_t p2;
		p2.x = 5;
		p2.x++;
		// compound struct-type definition
		typedef struct {
			int x;
			int y;
		} Point;
		Point p;
		p.x = 5;
		p.x++;
	#endif
}

/* 6. Array size constants
 *    C++ : Integer constant may (and should) be used to specify size of a constant-length array
 *    C   : Only a macro (or direct immediate) can specify size of a constant-length array
 */

#if HV_LANG == HV_CPP
	int const size = 100;
	int buf[size];         //: a constant-length array
#else
	#define SIZE 100
	int buf[SIZE];         //: a constant-length array
#endif

void array_size_tc() {
	int const size = 100;
	int vla[size];         //! a variable-length array (automatic storage only)
}

/* 7. The meaning of `auto` keyword
 *    C++ : allows compiler to determine the type of a variable automatically
 *    C   : specifies automatic storage duration for a variable (default for local variables)
 */
void auto_tc() {
#if HV_LANG == HV_CPP
	std::vector<int> vec(5, 10);
	auto it = vec.rbegin(); // auto determined type: std::vector<int>::reverse_iterator
	*it = 7;
#else
	auto int x = 5;          // set automatic storage for x (default for local variables)
#endif
}

void const_tc() {
	char c1 = 'v';
	char const c2 = 'c';

	char *pc = &c1;
	char *const cpc = &c1;
	char const *pcc = &c2;
	char const *const cpcc = &c2;
	*pc = *pcc;
}

void playground() {
#if HV_LANG == HV_CPP
	int x(5);
	printf("x is %d\n", x);
#endif
}


int main() {
	args_tc();
	sizeof_tc();
	nullptr_tc();
	voidarg_tc();
	compound_tc();
	array_size_tc();
	auto_tc();
	playground();
}

