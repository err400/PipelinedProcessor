# Makefile for Pipelined Processor

CXX = g++
CXXFLAGS = -Wall -std=c++11 -g

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

# Source files specific to each executable
NOFORWARD_SRCS = $(SRCS) noforward_main.cpp
FORWARD_SRCS = $(SRCS) forward_main.cpp

# Object files for each executable
NOFORWARD_OBJS = $(NOFORWARD_SRCS:.cpp=.o)
FORWARD_OBJS = $(FORWARD_SRCS:.cpp=.o)


# OBJS = $(SRCS:.cpp=.o)

# Targets
all: noforward forward

noforward: $(NOFORWARD_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Forwarding processor executable
forward: $(FORWARD_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(HEADERS_DIR) -c $< -o $@

clean:
	rm -f $(COMMON_OBJS) noforward_main.o forward_main.o noforward forward

.PHONY: all clean



# \
#        main.cpp