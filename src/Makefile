# Makefile for Pipelined Processor

CXX = g++
CXXFLAGS = -Wall -std=c++17 -g

# SRC_DIR = src
HEADERS_DIR = Headers
SRCS = Basic_Processor.cpp \
       InstructionFile.cpp \
       Memory.cpp \
       Processor.cpp \
       Registers.cpp \
       Utils.cpp 

COMMON_OBJS = Basic_Processor.o \
       InstructionFile.o \
       Memory.o \
       Processor.o \
       Registers.o \
       Utils.o

NOFORWARD_SRCS = $(SRCS) noforward.cpp
FORWARD_SRCS = $(SRCS) forward.cpp

NOFORWARD_OBJS = $(NOFORWARD_SRCS:.cpp=.o)
FORWARD_OBJS = $(FORWARD_SRCS:.cpp=.o)

all: noforward forward

noforward: $(NOFORWARD_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

forward: $(FORWARD_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(HEADERS_DIR) -c $< -o $@

clean:
	rm -f $(COMMON_OBJS) noforward.o forward.o noforward forward

.PHONY: all clean
