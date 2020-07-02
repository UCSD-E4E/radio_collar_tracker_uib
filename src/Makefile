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
# 07/02/20  NH  Rename to Makefile
# 07/01/20  NH  Initial Commit
# 
################################################################################

CFLAGS = -std=gnu11 -c -g -Wall

.PHONY: clean

main: main.o nmea.o compass_sim.o

clean:
	-rm *.o main