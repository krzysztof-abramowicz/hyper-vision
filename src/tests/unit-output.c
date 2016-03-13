/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Unit tests
 *     toolset: ISO C '99 + glib 2.0
 *     content: Test cases for the [output] module (text/x-c)
 *     version 1.0
 */

#include "output.h"
#include <glib.h>

// Test case for message level selection
void log_levels_sub1() {
	hvLogInit(HV_LOG_INFO, stdout);
	hvDebug("debug");
	hvInfo("info");
	hvWarning("warning");
}
void log_levels_sub2() {
	hvLogInit(HV_LOG_WARNING, stdout);
	hvDebug("debug");
	hvInfo("info");
	hvWarning("warning");
}
void log_levels_tc() {
	g_test_trap_subprocess("/hv/output/levels/subprocess/1", 0, 0);
	g_test_trap_assert_stdout_unmatched("*debug*");
	g_test_trap_assert_stdout("*info*");
	g_test_trap_assert_stdout("*warning*");
	g_test_trap_assert_passed();
	g_test_trap_subprocess("/hv/output/levels/subprocess/2", 0, 0);
	g_test_trap_assert_stdout_unmatched("*debug*");
	g_test_trap_assert_stdout_unmatched("*info*");
	g_test_trap_assert_stdout("*warning*");	
	g_test_trap_assert_passed();
}

// Manual test for message formatting
void log_colors_print_tc() {
	printf("\n");
	hvLogInit(HV_LOG_INFO, stdout);
	hvLogTabWidth(4);
	hvPrint("\tNumerical output test:");
	hvPrint("\t\tdec:", hvInt(128), "  hex:", hvHex(128), "  flt:", hvFlt(128));
	hvPrint("\tColourful output test:");
	hvPrint("\t\t",
		hvRed("dec:", hvInt(128)),
		hvGreen("  hex:", hvHex(128)),
		hvBlue("  flt:", hvFlt(128)),
		hvYellow(" str:", hvStr("seventh power of two"))
	);
	hvPrint("\tTabulated output test:");
	hvLogTabWidth(4);
	hvPrint("\t|\t|\t|\t|\ttab:4");
	hvLogTabWidth(5);
	hvPrint("\t|\t|\t|\t|\ttab:5");
	hvLogTabWidth(6);
	hvPrint("\t|\t|\t|\t|\ttab:6");
	hvLogTabWidth(4);
}

int main (int argc, char *argv[]) {
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/hv/output/levels", log_levels_tc);
	g_test_add_func("/hv/output/levels/subprocess/1", log_levels_sub1);
	g_test_add_func("/hv/output/levels/subprocess/2", log_levels_sub2);
	g_test_add_func("/hv/output/colors-print", log_colors_print_tc);	
	return g_test_run();
}

