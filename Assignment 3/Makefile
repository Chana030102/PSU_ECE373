# Aaron Chan
# ECE373 (Spring 2017)
# Assignment #3 module Makefile

obj-m = blinkLEDv3.o
KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build SUBDIRS=$(shell pwd) modules
clean:
	make -C /lib/modules/$(KVERSION)/build SUBDIRS=$(shell pwd) clean
