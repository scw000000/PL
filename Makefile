# This is a very simple makefile for building the Lisp interpreter
# project when using C++ on stdlinux. Feel free to add any improvements:
# e.g. pattern rules, automatic tracking of dependencies, etc. There
# is a lot of info about "make" on the web.

# C++ compiler
CXX = g++

# C++ compiler flags
CXXFLAGS = -g -Wall -std=c++11

# Creating a .o file
COMPILE = $(CXX) $(CXXFLAGS) -c

# Name of the executable; should match the contents of Runfile
EXE = Interpreter

# All object files
OBJS = main.o Scanner.o Parser.o ParserNode.o

# The first target is the one that is executed when you invoke
# "make". The line describing the action starts with <TAB>. Variable
# "$@" stands for the current target. 

$(EXE) : $(OBJS) # depends on all object files
	$(CXX) $^ -o $@

# An object file is dependent on the corresponding source file

%.o : %.cc
	$(COMPILE) -o $@ $<

main.o : Scanner.h Parser.h

Scanner.o : Scanner.h

Parser.o : Parser.h ParserNode.h

ParserNode.o : ParserNode.h