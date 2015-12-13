#
# @file make/app.mk
# @version 1.0
#
# @section License
# Copyright (C) 2014-2015, Erik Moqvist
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# This file is part of the Simba project.
#

.PHONY: all clean new run run-debugger help

VERSION ?= 0.0.0

# files and folders
OBJDIR = obj
DEPSDIR = deps
GENDIR = gen
INC += . $(SIMBA)/src
SRC += main.c
SRC_FILTERED = $(filter-out $(SRC_IGNORE),$(SRC))
CSRC += $(filter %.c,$(SRC_FILTERED))
COBJ = $(patsubst %,$(OBJDIR)/%,$(notdir $(CSRC:%.c=%.o)))
OBJ = $(COBJ)
GENCSRC = $(GENDIR)/simba_gen.c
GENOBJ = $(patsubst %,$(OBJDIR)/%,$(notdir $(GENCSRC:%.c=%.o)))
SETTINGS_INI ?= $(SIMBA)/make/settings.ini
SETTINGS_H = settings.h
SETTINGS_BIN = settings.bin
EXE = $(NAME).out
RUNLOG = run.log
CLEAN = $(OBJDIR) $(DEPSDIR) $(GENDIR) $(EXE) $(RUNLOG) size.log \
        coverage.log coverage.xml gmon.out *.gcov profile.log \
	index.*html $(SETTINGS_H) $(SETTINGS_BIN)

# configuration
TOOLCHAIN ?= gnu
CFLAGS += $(INC:%=-I%) $(CFLAGS_EXTRA)
ifeq ($(NDEBUG),yes)
  CDEFS += -DNDEBUG
endif
ifeq ($(NPROFILE),yes)
  CDEFS += -DNPROFILE
endif
CDEFS +=  -DARCH_$(UPPER_ARCH) -DMCU_$(UPPER_MCU) \
          -DBOARD_$(UPPER_BOARD) -DVERSION=$(VERSION)
CFLAGS += $(CDEFS)
LDFLAGS += $(LDFLAGS_EXTRA)
SHELL = /usr/bin/env bash

all: $(EXE) $(SETTINGS_BIN)

# layers
BOARD.mk ?= $(SIMBA)/src/boards/$(BOARD)/board.mk
include $(BOARD.mk)
MCU.mk ?= $(SIMBA)/src/mcus/$(MCU)/mcu.mk
KERNEL.mk ?= $(SIMBA)/src/kernel/kernel.mk
DRIVERS.mk ?= $(SIMBA)/src/drivers/drivers.mk
SLIB.mk ?= $(SIMBA)/src/slib/slib.mk

include $(MCU.mk)
include $(KERNEL.mk)
include $(DRIVERS.mk)
include $(SLIB.mk)

UPPER_ARCH = $(shell echo $(ARCH) | tr a-z A-Z)
UPPER_MCU = $(shell echo $(MCU) | tr a-z A-Z | tr - _ | tr / _)
UPPER_BOARD = $(shell echo $(BOARD) | tr a-z A-Z)

RUNSCRIPT = $(SIMBA)/make/$(TOOLCHAIN)/$(ARCH).py

clean:
	@echo "Cleaning"
	rm -rf $(CLEAN)

new:
	$(MAKE) clean
	$(MAKE) all

run: all
	@echo "Running $(EXE)"
	set -o pipefail ; stdbuf -i0 -o0 -e0 $(RUNSCRIPT) run ./$(EXE) $(SIMBA) $(RUNARGS) | tee $(RUNLOG)

dump:
	set -o pipefail ; $(RUNSCRIPT) dump ./$(EXE) $(SIMBA) $(RUNARGS)

report:
	@echo "$(NAME):"
	grep "exit: test_" $(RUNLOG) | python $(SIMBA)/make/color.py || true

test: run
	$(MAKE) report

run-debugger: all
	set -o pipefail ; stdbuf -i0 -o0 -e0 $(RUNSCRIPT) debugger ./$(EXE) $(SIMBA) $(RUNARGS) | tee $(RUNLOG)

profile:
	set -o pipefail ; $(RUNSCRIPT) profile ./$(EXE) $(SIMBA) | tee profile.log

coverage:
	set -o pipefail ; $(RUNSCRIPT) coverage ./$(EXE) $(SIMBA) | tee coverage.log

size:
	set -o pipefail ; $(SIZECMD) | tee size.log

jenkins-coverage:
	$(RUNSCRIPT) jenkins-coverage ./$(EXE) > coverage.xml

release:
	env NDEBUG=yes NPROFILE=yes $(MAKE)

$(EXE): $(OBJ) $(GENOBJ)
	@echo "Linking $@"
	$(LD) -o $@ $^ $(LDFLAGS)

$(SETTINGS_BIN) $(SETTINGS_H): $(SETTINGS_INI)
	@echo "Generating $@ from $<"
	$(SIMBA)/src/kernel/tools/settings.py $(SETTINGS_INI) $(ENDIANESS)

define COMPILE_template
-include $(patsubst %.c,$(DEPSDIR)/%.o.dep,$(notdir $1))
$(patsubst %.c,$(OBJDIR)/%.o,$(notdir $1)): $1 $(SETTINGS_H)
	@echo "Compiling $1"
	mkdir -p $(OBJDIR) $(DEPSDIR) $(GENDIR)
	$$(CC) $$(CFLAGS) -DMODULE_NAME=$(notdir $(basename $1)) -D__SIMBA_GEN__ \
	    -E -o $(patsubst %.c,$(GENDIR)/%.o.pp,$(notdir $1)) $$<
	$$(CC) $$(CFLAGS) -DMODULE_NAME=$(notdir $(basename $1)) -o $$@ $$<
	gcc -MM -MT $$@ $$(filter -I% -D% -O%,$$(CFLAGS)) -o $(patsubst %.c,$(DEPSDIR)/%.o.dep,$(notdir $1)) $$<
endef
$(foreach file,$(CSRC),$(eval $(call COMPILE_template,$(file))))

$(GENOBJ): $(OBJ)
	$(SIMBA)/src/kernel/tools/gen.py $(NAME) $(VERSION) $(BOARD_DESC) $(MCU_DESC) \
	    $(GENCSRC) $(OBJ:$(OBJDIR)/%=$(GENDIR)/%.pp)
	@echo "Compiling $(GENCSRC)"
	$(CC) $(CFLAGS) -o $@ $(GENCSRC)

-include local.mk
include $(SIMBA_GLOBAL_MK)

valgrind:
	valgrind --leak-check=full ./$(EXE)

CPPCHECK_ENABLE = warning performance portability information

cppcheck:
	(for path in $(CSRC); do echo $$path; done) | \
	cppcheck --std=c99 $(CPPCHECK_ENABLE:%=--enable=%) $(INC:%=-I%) $(CDEFS) \
	--file-list=- --template=gcc --error-exitcode=1

help:
	@echo "--------------------------------------------------------------------------------"
	@echo "  target                      description"
	@echo "--------------------------------------------------------------------------------"
	@echo "  all                         compile and link the application"
	@echo "  clean                       remove all generated files and folders"
	@echo "  new                         clean + all"
	@echo "  run                         run the application"
	@echo "  run-debugger                run the application in the debugger, break at main"
	@echo "  report                      print test report"
	@echo "  test                        run + report"
	@echo "  release                     compile with NDEBUG=yes and NPROFILE=yes"
	@echo "  size                        print executable size information"
	@IFS=$$'\n' ; for h in $(HELP_TARGETS) ; do \
	  echo $$h ; \
	done
	@echo "  help                        show this help"
	@echo "--------------------------------------------------------------------------------"
	@echo "  variable                    description"
	@echo "--------------------------------------------------------------------------------"
	@echo "  NDEBUG                      yes - build without debug information"
	@echo "  NPROFILE                    yes - build without profiling information"
	@IFS=$$'\n' ; for h in $(HELP_VARIABLES) ; do \
	  echo $$h ; \
	done
	@echo
