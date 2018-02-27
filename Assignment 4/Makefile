# Aaron Chan
# ECE373 (Spring 2017)
# Assignment #4 module Makefile

obj-m = blinkLEDv4.o
KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build SUBDIRS=$(shell pwd) modules
clean:
	make -C /lib/modules/$(KVERSION)/build SUBDIRS=$(shell pwd) clean
