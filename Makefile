
# ┌──────────────────────────────────────────────────────────────────────────────────┐
# │ Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com) │
# │   project: HyperVision — Architectonic Rendering Engine                          │
# │             └─HV Application Framework                                           │
# │                └─Build automation                                                │
# │   toolset: GNU Make 3.82+                                                        │
# │   content: Application building rules (text/x-makefile)                          │
# │   version: 1.2 OS                                                                │
# └──────────────────────────────────────────────────────────────────────────────────┘

#### CONFIGURATION SECTION ###

# Core components and dependences
program := hv
process := output.in matrix.in shader.in model.in
sources := output.c matrix.c opengl.c shader.c model.c main.c
xdepend := output+opengl matrix+output
testing := matrix-test

# General naming conventions 
lnkchr := +
incdir := inc
srcdir := src
objdir := obj
tstdir := $(srcdir)/tests

# Compilation and linking flags
CC := gcc
CPPFLAGS := -I$(incdir)
CFLAGS := -Wall -Wextra -std=c99 -pedantic -O2
LDFLAGS := -lGLEW -lGLU -lGL -lglut -lm
ECHO := `which echo` -e
DEFAULT_GOAL: $(program)


#### PROCESSING SECTION ####  ────(do not edit below this line)────

# Declare helpful constants
empty :=
space := $(empty) $(empty)
col := :
tab := $(empty)    $(empty)

# Test for string equality and set membership
equals = $(if $(subst $1,,$2),$(empty),true)
exists = $(if $(filter $1,$2),true,$(empty))

# Extract first/second member of a dependency token
pred = $(firstword $(subst +, ,$1))
succ = $(lastword $(subst +, ,$1))

# Construct paths for intermediate components
headers := $(addprefix $(incdir)/, $(process:.in=.h))
objects := $(addprefix $(objdir)/, $(sources:.c=.o))

# Rebuild prerequisites to expose module interdependencies
discover = $(strip $(foreach mod,$(basename $1),\
              $(subst $(space),,$(objdir)/$(mod)\
              $(foreach dep,$(xdepend),\
                  $(and\
                      $(call equals,$(call pred,$(dep)),$(mod)),\
                      $(call exists,$(call succ,$(dep)),$(basename $1)),\
                      $(lnkchr)$(call succ,$(dep))\
                  )\
               ).o)\
           ))

# Set up linking switches for required libraries
libs = $(strip $(foreach lib,$1,\
          $(if $(call equals,gobj,$(lib)),\
             `pkg-config --cflags --libs gobject-2.0`,\
             $(if $(call equals,glib,$(lib)),\
                `pkg-config --cflags --libs glib-2.0`,\
                -l$(lib)))\
        ))

# Create compilation rules for interdependent module objects
define rule
$(objdir)/$1$(lnkchr)$2.o: $(srcdir)/$1.c $(incdir)/$1.h $(incdir)/$2.h | $(objdir)
	$(CC) $(CPPFLAGS) $(CFLAGS) -include $(incdir)/$2.h -c $(srcdir)/$1.c -o $(objdir)/$1$(lnkchr)$2.o

endef
$(foreach dep,$(xdepend),$(eval $(call rule,$(call pred,$(dep)),$(call succ,$(dep)))))


#### RULES' SECTION ####

# TODO: libhv.a, project statistics, automated tests

# Initial definitions
.PHONY: all mostlyclean clean tar self-test
.PRECIOUS: $(headers)

all: $(program) $(testing)

# Rules for main program and intermediate objects
$(program): $(call discover,$(sources))
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(objdir)/%.o: $(srcdir)/%.c $(incdir)/%.h | $(objdir)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(objdir):
	mkdir $(objdir)

# Rule for headers' preprocessing with hvCPP
export srcdir
export incdir
$(incdir)/%.h: %.in
	./hvCPP.pl -I $< > $@

# Rules for building UNIT and IDEA tests
tests: $(testing)

glx-test: $(objdir)/output.o $(tstdir)/idea-glx.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(call libs,X11 GL GLU)

output-test: $(objdir)/output.o $(tstdir)/unit-output.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(call libs,glib)

matrix-test: $(call discover,output matrix) $(wildcard $(tstdir)/unit-matrix*[cs])
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(call libs,m glib)

gobject-test: $(tstdir)/unit-gobject.c 
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(call libs,gobj)

view-diff: $(tstdir)/view-diff.c
	@ $(ECHO) "\e[32m ---- ISO C++'11 version ---- \e[0m"
	- g++ $(CPPFLAGS) -Wall -Wextra -D HV_LANG=2 -std=c++11 -pedantic -O2 -o view-diff-cpp $^
	@ $(ECHO) "\e[32m ---- ISO C'99 version ---- \e[0m"
	- gcc $(CPPFLAGS) -Wall -Wextra -D HV_LANG=1 -std=c99   -pedantic -O2 -o view-diff-c $^

view-cpp: $(tstdir)/view-cpp.cpp
	g++ $(CPPFLAGS) -Wall -Wextra -std=c++11 -pedantic -O2 -o $@ $^

# Rules for cleaning and backing up
mostlyclean:
	- rm -f $(program) $(testing)
	- rm -R -f $(objdir)

clean: mostlyclean
	- rm -f $(headers)
	- rm -f $(srcdir)/*_p.h
	- find ./ -name '*~' -delete

tar: clean
	rm -f *.tar.bz2
	tar -cjf HV-DS1-`date -I`.tar.bz2 *

# Makefile's self test
self-test: ;
ifeq (self-test,$(MAKECMDGOALS))
   $(info Makefile self test$(col))
   $(info objects = $(objects))
   cmd = $(wildcard $(tstdir)/unit-matrix*[cs])
   $(info 1. Wildcards$(col))
   $(info $(tab)$(value cmd))
   $(info $(tab)$(cmd))
   cmd = $(call libs,m GL glib)
   $(info 2. Linking setup$(col))
   $(info $(tab)$(value cmd))
   $(info $(tab)$(cmd))
   cmd = $(call discover,$(sources))
   $(info 3. Interdependence discovery$(col))
   $(info $(tab)$(value cmd))
   $(info $(tab)$(cmd))
endif


#### CHANGELOG ####

#   Changes in version 1.2:
#     - Automatic discovery of module interdependences
#       (to preserve cross-module features in seperate compilation),
#     - Introduction of the three-part Makefile's layout
#       (configuration/processing/rules' sections),
#     - Consistent variable naming and meaningful comments.
	
