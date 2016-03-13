# Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
#     project: HyperVision — Architectonic Rendering Engine
#               └─Design directions' tests
#     toolset: GNU Make 3.82+
#     content: GLX context creation test (text/x-c)
#     version: 1.0 OS

nil :=
col := :
ind := $(nil)    $(nil)

vars_make := MAKE MAKEFLAGS MAKELEVEL MAKE_RESTARTS MAKEFILE_LIST
vars_conf := .LIBPATTERNS .DEFAULT_GOAL .FEATURES srcdir
vars_impl := CC CFAGS CXX CXXFLAGS CPP CPPFLAGS LDFAGS 

print = $(info $(ind)$(1): $($(1)))

$(info Make variables$(col))
$(foreach var,$(vars_make),$(call print,$(var)))

$(info Configuration variables$(col))
$(foreach var,$(vars_conf),$(call print,$(var)))

$(info Variables for implicit rules$(col))
$(foreach var,$(vars_impl),$(call print,$(var)))

.PHONY: list
list:
	@ echo "Shell variables:"
	@ echo \sSHELL: $(SHELL)
	@ echo \sSHELLFLAGS: $(SHELLFLAGS)


