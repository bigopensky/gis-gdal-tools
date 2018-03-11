### Makefile ---------------------------------------------------
## Author:  alex.weidauer@huckfinn.de
## Version: $Id: Makefile,v 0.1 2013/10/02 14:41:26 huckfinn Exp $

# -------------------------------------------------------------
# Libraries entires
# -------------------------------------------------------------
LPATH   = -L/usr/lib \
	  -L/usr/local/lib \
	  -L./lib

# -------------------------------------------------------------
# Include pathes
# -------------------------------------------------------------
IPATH   = -I/usr/include/ \
	  -I/usr/local/include\
	  -I./include

# -------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------
CFLAGS  = -std=c99 -pedantic
LMATH   = -lgsl -lblas -lm
LGDAL   = -lgdal

# -------------------------------------------------------------
# Directories
# -------------------------------------------------------------
BUILD   = ./build
DOC     = ./docs
MAN     = ./man
SRC     = ./src/ifgdv
INC     = ./include/ifgdv

# -------------------------------------------------------------
# Management stuff
# -------------------------------------------------------------
all:	$(BUILD)/gtif-cut \
	$(BUILD)/gtif-pos-read

.PHONY: clean
clean:
	@echo "..CLEANING $(BUILD)"
	rm $(BUILD)/*

# -------------------------------------------------------------
# Tools
# -------------------------------------------------------------

$(BUILD)/gtif-cut: $(BUILD)/alg.o $(SRC)/gtif-cut.c
	   gcc $(IPATH) $(LPATH) $(LGDAL) $(LMATH) $(CFLAGS) -o $@ $^

$(BUILD)/gtif-pos-read: $(BUILD)/alg.o $(SRC)/gtif-pos-read.c
	   gcc $(IPATH) $(LPATH) $(LGDAL) $(LMATH) $(CFLAGS) -o $@ $^

$(BUILD)/alg.o: $(SRC)/alg.c
	gcc  $(IPATH) $(LPATH) $(LMATH) $(CFLAGS) -o $@ -c $^

