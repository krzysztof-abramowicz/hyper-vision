/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Design directions' tests
 *     toolset: ISO C '99 (CPP)
 *     content: Macros for function overloading and "accessor methods'" dispatching (text/x-h)
 *     version: 1.0 OS
 *
 *  Run this test with `gcc -E idea-overload.h` command
 */

#define HV_COMMA(...) ,
#define HV_JOIN2(_1, _2) _1 ## _2
#define HV_JOIN3(_1, _2, _3) _1 ## _2 ## _3

/* HV_COUNT(list) -- count arguments ignoring empty case */
#define HV_MULTI(...) HV_COUNT_IMPL(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 0)
#define HV_COUNT(...) HV_COUNT_IMPL(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)
#define HV_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _N, ...) _N

/* HV_EMPTY(list) -- checks if argument list is empty */
#define HV_EMPTY(...)                      \
	HV_EMPTY_IMPL(                         \
 		HV_MULTI(HV_COMMA __VA_ARGS__),    \
		HV_MULTI(HV_COMMA __VA_ARGS__ ())  \
	)
#define HV_EMPTY_IMPL(_1, _2) HV_MULTI(HV_JOIN3(HV_EMPTY_CASE_, _1, _2))
#define HV_EMPTY_CASE_01 ,


/* HV_VARGS(list) -- count arguments accounting empty case */
#define HV_ARITY(...) \
	HV_ARITY_IMPL( \
		HV_EMPTY(__VA_ARGS__), \
		HV_COUNT(__VA_ARGS__)  \
	)
#define HV_ARITY_IMPL(_1, _2) HV_ARITY_EVAL(HV_JOIN2(HV_ARITY_CASE_, _1), _2)
#define HV_ARITY_EVAL(switch, argc) switch(argc)
#define HV_ARITY_CASE_0(N) N
#define HV_ARITY_CASE_1(N) 0


#define hvModelName(...) HV_ACCESSOR(hvModelName, __VA_ARGS__)

#define HV_DISPATCH(name, ...) HV_DISPATCH_JOIN(name, HV_ARITY(__VA_ARGS__), __VA_ARGS__)
#define HV_DISPATCH_JOIN(name, count, ...) HV_DISPATCH_EVAL(HV_JOIN2(name, count), __VA_ARGS__)
#define HV_DISPATCH_EVAL(name, ...) name(__VA_ARGS__)

#define hvModelName(...) HV_ACCESSOR(hvModelName, __VA_ARGS__)

#define HV_ACCESSOR(name, ...) HV_ACCESSOR_IMPL1(name, HV_ARITY(__VA_ARGS__), __VA_ARGS__)
#define HV_ACCESSOR_IMPL1(name, argc, ...) HV_ACCESSOR_IMPL2(name, HV_JOIN2(HV_ACCESSOR_CASE, argc), __VA_ARGS__)
#define HV_ACCESSOR_IMPL2(name, type, ...) HV_ACCESSOR_IMPL3(HV_JOIN2(name, type), __VA_ARGS__)
#define HV_ACCESSOR_IMPL3(name, ...) name(__VA_ARGS__)
#define HV_ACCESSOR_CASE1 Get
#define HV_ACCESSOR_CASE2 Set

#define HV_ACCESSOR_CASE0 HV_ACCESSOR_ERROR
#define HV_ACCESSOR_CASE3 HV_ACCESSOR_ERROR
#define HV_ACCESSOR_CASE4 HV_ACCESSOR_ERROR
#define HV_ACCESSOR_ERROR \
	_Pragma("message \"Incorrect number of arguments for an accessor method\"") _HERE->

Positive argument count
8 == HV_COUNT(1, 2, 3, 4, 5, 6, 7, 8)
4 == HV_COUNT(1, 2, 3, 4)
2 == HV_COUNT(1, 2)
1 == HV_COUNT(1)

Empty argument detection
func(a,b,c)	HV_ARITY(a,b,c)
func(a,b)	HV_ARITY(a,b)
func(a)		HV_ARITY(a)
func(())    HV_ARITY(())
func((()))  HV_ARITY((()))
func(()())  HV_ARITY(()())
func((),()) HV_ARITY((),())
func(,)     HV_ARITY(,)
func()      HV_ARITY()

Function overloading
hvModelName(model)
hvModelName(model, "name")


