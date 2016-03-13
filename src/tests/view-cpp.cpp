/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─View tests
 *     toolset: ISO C++'11
 *     content: View on C++'11 syntax and features
 *     version: 1.0
 */

#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

// Scope resolution operator
int i = 5;

void scope_tc() {
	cout << "> Scope resolution\n";
	int i = 4;
	{
		int i = 3;
		cout << "\t\ti = " << i << ", ::i = " << ::i << '\n';
	}
}

/* virtual vs non-virtual structs/classes
 *
 *
 */
void struct_size_tc() {
	struct S {
		int x, y;
	};
	struct C {
		int x, y;
		void print() {
			cout << x << y << "\n";
		}
	};
	struct V {
		int x, y;
		virtual void print() {
			cout << x << y << "\n";
		}
	};
	cout << "> vtab existence and impact:\n";
	cout << "\t\tS: " << sizeof(S)
	     << ", C: " << sizeof(C)
	     << ", V: " << sizeof(V) << "\n";
}

/* Argument-dependent lookup (ADL)
 *
 */

#define HV_IDENTIFY cout << "\t\t" << __PRETTY_FUNCTION__ << '\n';

namespace A {

	enum T { ARG };
	void function(T) { HV_IDENTIFY }
	void operator+(T, int) { HV_IDENTIFY }

	struct X {
		enum T { ARG };
		void method(T) { HV_IDENTIFY }
		void operator+(int) { HV_IDENTIFY }
	};
}

namespace B {

	enum T { ARG };
	void function(T) { HV_IDENTIFY }
	void operator+(T, int) { HV_IDENTIFY }

    struct X {
		enum T { ARG };
		void function() { HV_IDENTIFY }
		void operator+(int) { HV_IDENTIFY }
		void search();
	};

	void X::search() {
		// member operator
		 *this          + 16 ;
		(*this).operator+(16);
		        operator+(16);
		// non-member operator
		          B::ARG + 16;
B::		operator+(B::ARG,  16); // "class-major" name lookup: no ADL
B::		 function(B::ARG);      //

		// foreign operator
		          A::ARG + 16;
A::		operator+(A::ARG,  16); // "class-major" name lookup: no ADL
A::		 function(A::ARG);      //
	}
}

void lookup_tc() {
	cout << "> Argument-dependent lookup\n";

	          B::ARG + 16;
	operator+(B::ARG,  16); // classic name lookup: with ADL
	 function(B::ARG);      //	

    B::X x;
    x.search();
}

/* IOS formatting flags
 *
 */

void format_tc() {
	ios::fmtflags initial = cout.flags();
	cout << "> IOS formatting:\n";

	// adjustfield
	cout << "\tadjustfield (flags)\n\t\t|";
	cout.setf(ios::right, ios::adjustfield);
	cout.width(8);
	cout << -42 << '|';
	cout.setf(ios::left, ios::adjustfield);
	cout.width(8);
	cout << -42 << '|';
	cout.setf(ios::internal, ios::adjustfield);
	cout.width(8);
	cout << -42 << '|' << '\n';
	
	cout << "\tadjustfield (manip)\n\t\t|"
	     << right    << setw(8) << -32 << '|'
	     << left     << setw(8) << -32 << '|'
	     << internal << setw(8) << -32 << '|' << '\n';

	cout.flags(initial);
	
	// basefield + showbase
	cout << "\tbasefield + showbase (flags)\n\t\t";
	cout.unsetf(ios::showbase);
	cout.setf(ios::dec, ios::basefield);
	cout << 16 << ", ";
	cout.setf(ios::hex, ios::basefield);
	cout << 16 << ", ";
	cout.setf(ios::oct, ios::basefield);
	cout << 16 << "; ";
	cout.setf(ios::showbase);
	cout.setf(ios::dec, ios::basefield);
	cout << 16 << ", ";
	cout.setf(ios::hex, ios::basefield);
	cout << 16 << ", ";
	cout.setf(ios::oct, ios::basefield);
	cout << 16 << '\n';

	cout << "\tbasefield + showbase (manip)\n\t\t";
	cout << noshowbase
	     << setbase(10) << 16 << ", "
	     << setbase(16) << 16 << ", "
	     << setbase(8)  << 16 << "; "
	     << showbase
	     << setbase(10) << 16 << ", "
	     << setbase(16) << 16 << ", "
	     << setbase(8)  << 16 << '\n';

	cout.flags(initial);

	// floatfield + showpoint
	cout << "\tfloatfield  + showpoint(flags)\n\t\t";
	cout.precision(4);

	cout << 3.f << ", " << 3.1f << ", " << 3.14f << "\n\t\t";
	cout.setf(ios::showpoint);
	cout << 3.f << ", " << 3.1f << ", " << 3.14f << "\n\t\t";	
	cout.setf(ios::fixed, ios::floatfield);
	cout << 3.f << ", " << 3.1f << ", " << 3.14f << "\n\t\t";
	cout.setf(ios::scientific, ios::floatfield);
	cout << 3.f << ", " << 3.1f << ", " << 3.14f << '\n';

	cout.flags(initial);

	cout << "\tfloatfield  + showpoint(manip)\n\t\t"
	     <<               3.f << ", " << 3.1f << ", " << 3.14f << "\n\t\t"
	     << showpoint  << 3.f << ", " << 3.1f << ", " << 3.14f << "\n\t\t"
	     << fixed      << 3.f << ", " << 3.1f << ", " << 3.14f << "\n\t\t"
	     << scientific << 3.f << ", " << 3.1f << ", " << 3.14f << '\n';
}

/* const/volatile method overloading
 *
 */

// SO: why do I need to link -lm for math (standard library), while unistd does not need explicit linking
// SO: how to list all lookup possibilities

/* Move semantics (perfect forwarding)
 * src: C++ Annotations
 */

struct Data {
	char *text;
	size_t size;
	void copy(Data const &other) {
		text = strdup(other.text);
		size = strlen(text);
	}
};

Data dataFactory(char const *txt) {
	return {strdup(txt), strlen(txt)};
}

void move_tc() {9-16 
	cout << "> Move semantics:\n";
	Data d1 = {strdup("hello"), strlen("hello")};
	Data d2;
	d2.copy(d1);
	Data d3;
	d3.copy(dataFactory("hello"));      // 2
}

/* Type inference
 *
 */

using FUN = auto (double, int) -> unsigned long long int;
// auto, decltype

/* Literals (raw strings, UTF-8 strings, unsigned long ints, etc.)
 *
 */

#define ESC "\x1B" 

int main() {
	cout << ESC"7" ESC"[3g" ESC"[4C" ESC"H" ESC"[4C" ESC"H" ESC"[4C" ESC"H" ESC"[4C" ESC"H" ESC"8";
	cout << "VIEW: C++'11 syntax and features\n";
	scope_tc();
	format_tc();
	lookup_tc();
	struct_size_tc();
}

// ::i global, higher scope
// ref vs. *
// virual inheritance
// the concept of encapsulation

