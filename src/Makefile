################################################################################
# @file makefile
# 
# @author Nathan Hui, nthui@eng.ucsd.edu
# 
# @description
# UI Board FW Makefile.  This makefile expects GNU Make and GCC
# 
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU general Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
# details.
# 
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>
# 
# 
# DATE      WHO DESCRIPTION
# -----------------------------------------------------------------------------
# 08/09/20  NH  Added all, fixed simulation build objects
# 08/07/20  NH  Added UIB simulation build
# 07/02/20  NH  Rename to Makefile, added dependency checking
# 07/01/20  NH  Initial Commit
# 
################################################################################

CC=gcc
SRCS = main.c nmea.c voltage_sim.c compass_sim.c uibWinBuild.c serial_sim.c
CFLAGS = -std=gnu11 -c -g -Wall
LDLIBS = -lpthread

SIM_OBJS = compass_sim.o voltage_sim.o serial_sim.o

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c


.PHONY: clean all

all: uibWinBuild main

uibWinBuild: uibWinBuild.o nmea.o $(SIM_OBJS)

main: main.o nmea.o $(SIM_OBJS)

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

clean:
	-rm *.o main

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))