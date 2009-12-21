#	arm_flash Makefile
#
#
# arm_flash is a flashing linux command line utility
# for arm based (micro)processors
#
# currently supported mcus:
#	- Phillips
#		* LPC2103
#
#
#
# Author: Bc. Gabriel Zabusek		25.11.2008

#compiler definitions
CC  = gcc
CXX = g++
AS  = as
LD  = ld

BASE_DIR = ./
CORE_DIR = $(BASE_DIR)core/
FIRMWARE_DIR = $(BASE_DIR)firmware/
DEVICE_DIR = $(BASE_DIR)device/
TOOLS_DIR = $(BASE_DIR)tools/
MAN_DIR = $(BASE_DIR)man/
CATMAN_DIR = $(BASE_DIR)catman/


CCWARNINGS = -Wall -Wextra -Werror -Wno-unused-parameter -Wmissing-prototypes
CXXWARNINGS = -Wall -Wextra -Werror -Wno-unused-parameter

#compiler settings for C files
CCFLAGS  = -O3 -pipe -ansi -pedantic -pthread \
	   -I$(BASE_DIR) \
	   -I$(CORE_DIR) \
	   -I$(FIRMWARE_DIR) \
	   -I$(DEVICE_DIR) \
	   -I$(TOOLS_DIR) \
	     $(CCWARNINGS)

#compiler settings for C++ files
CXXFLAGS = -O3 -pipe -ansi -pedantic -pthread \
	   -I$(BASE_DIR) \
	   -I$(CORE_DIR) \
	   -I$(FIRMWARE_DIR) \
	   -I$(DEVICE_DIR) \
	   -I$(TOOLS_DIR) \
	     $(CXXWARNINGS)

#source files
CORE_SRC = \
	$(CORE_DIR)serial.cxx \
	$(FIRMWARE_DIR)CFirmwareHEX32.cxx \
	$(CORE_DIR)cmdargs.c \
	$(DEVICE_DIR)CDeviceBase.cxx \
	$(DEVICE_DIR)CDeviceLPC2103.cxx \
    $(DEVICE_DIR)CDeviceSupport.cxx \
    $(DEVICE_DIR)CFlashingStatus.cxx \
	$(TOOLS_DIR)UUcoder.cxx \
    $(TOOLS_DIR)CFlashData.cxx \
    $(TOOLS_DIR)CThreadDispatcher.cxx \
	$(CORE_DIR)main.cxx 

CORE_BIN = armflash

#objects
CORE_OBJ := $(addsuffix .o,$(basename $(CORE_SRC)))


all: $(CORE_BIN) man

#default compiling rule for C files
# NOTE: i am using g++ also for .C files as we are mixing c and c++ objects together and c++ mangles names
#	i wanted to avoid using extern "C" stuff etc.
%.o: %.c
	@$(CXX) $(CXXFLAGS) -c -o $@ $<
	@echo "Compiling" $<

#default compiling rule for C++ files
%.o: %.cxx
	@$(CXX) $(CXXFLAGS) -c -o $@ $<
	@echo "Compiling" $<

$(CORE_BIN): $(CORE_OBJ)
	@$(CXX) $(CXXFLAGS) -o $@ $(CORE_OBJ)
	@echo "Linking final binary"
	
man: catman/armflash.1 catman/armflash.ps
	@echo "Generating manpages"


catman/armflash.1:
	@groff -man -Tascii man/armflash.1 > catman/armflash.1

catman/armflash.ps:
	@groff -man man/armflash.1 > catman/armflash.ps

install:
	cp -f ./armflash /usr/bin/armflash
	cp -f ./catman/armflash.1 /usr/share/man/man1

uninstall:
	rm -f /usr/bin/armflash
	rm -f /usr/share/man/man1/armflash.1

clean:
	rm -f $(CORE_BIN)
	rm -f $(CORE_DIR)*.o
	rm -f $(FIRMWARE_DIR)*.o
	rm -f $(DEVICE_DIR)*.o
	rm -f $(TOOLS_DIR)*.o
	rm -f $(CATMAN_DIR)*

