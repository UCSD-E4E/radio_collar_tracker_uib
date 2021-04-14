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
# 08/30/20  NH  Initial commit
################################################################################

CC = avr-gcc
LD = avr-gcc

DEVICE		=	atmega32u4
CLOCK		=	16000000

MACRO_DEFS	=	-DF_CPU=$(CLOCK) -DUSB_VID=0x2341 -DUSB_PID=0x8036 -DARDUINO=105 -D__PROG_TYPES_COMPAT__

CFLAGS = -std=gnu11 -c -g -Wall -Wextra -ffunction-sections -fdata-sections $(MACRO_DEFS) -mmcu=$(DEVICE) -Os 
LDFLAGS = -Os -Wl,--gc-sections -mmcu=$(DEVICE) -lm
LDLIBS = -lm

SRCS = uib.c LED_module.c CDC.c cutils.c serial.c USBCore.c sensor_module.c \
	status_decoder.c sensor_encoder.c nmea.c compass.c voltage.c I2C.c
COBJS = $(SRCS:%.c=%.o)

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

.PHONY: clean all
all: uib.elf

clean:
	-rm -f *.o *.elf

%.o: %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(CC) $(CFLAGS) $<
	
uib.elf: $(COBJS)
	${LD} -o $@ $^ $(LDFLAGS)
	
load: uib.elf
	avrdude -p m32u4 -c dragon_isp -B 4 -P usb -U flash:w:$<