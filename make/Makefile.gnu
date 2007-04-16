#
# settings for gnu-compatible platforms
#

#
# Tools
#
CXX = g++
CC = gcc
MAKEDEPEND = $(CXX) -MM -MG
CMAKEDEPEND = $(CC) -MM -MG

RANLIB = ranlib


#
# General Flags
#

DEBUG_CFLAGS = -g
OPT_CFLAGS = -O3
PROFILE_CFLAGS = -pg
PROFILE_LFLAGS = -pg

ifdef CHPL_GCOV
CFLAGS += -fprofile-arcs -ftest-coverage
LDFLAGS += -fprofile-arcs
endif


#
# Flags for compiler, runtime, and generated code
#

COMP_CFLAGS = $(CFLAGS)
RUNTIME_CFLAGS = $(CFLAGS)
GEN_CFLAGS = 

#
# Flags for turning on warnings for C++/C code
#
WARN_CXXFLAGS = -Wall -Werror
WARN_CFLAGS = $(WARN_CXXFLAGS) -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes


GNU_GCC_MAJOR_VERSION = $(shell $(CC) -dumpversion | awk '{split($$1,a,"."); printf("%s", a[1]);}')
GNU_GCC_MINOR_VERSION = $(shell $(CC) -dumpversion | awk '{split($$1,a,"."); printf("%s", a[2]);}')

#
# this is fragile, but version 3.3.4 doesn't support these flags
#
ifeq ($(GNU_GCC_MAJOR_VERSION),3)
ifeq ($(GNU_GCC_MINOR_VERSION),4)
WARN_CFLAGS += -Wdeclaration-after-statement -Wnested-externs 
endif
endif


#
# developer settings
#
ifdef CHPL_DEVELOPER
COMP_CFLAGS += $(WARN_CXXFLAGS)
RUNTIME_CFLAGS += $(WARN_CFLAGS)
endif
