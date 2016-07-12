# Makefile for a UCI-based analyser.
# Copyright (C) David J. Barnes 2013-2014
# d.j.barnes@kent.ac.uk
# http://www.cs.kent.ac.uk/~djb/
# Available from: http://www.cs.kent.ac.uk/~djb/chessplag/

CC=g++
CFLAGS=-c -g -Wall -O3 -D__unix__
OBJS=analyse.o evaluation.o engine.o utils.o
TARGET=analyse

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

engine.o : engine.cpp engine.h
evaluation.o : evaluation.cpp evaluation.h utils.h
analyse.o : analyse.cpp engine.h evaluation.h utils.h
utils.o : utils.cpp utils.h
