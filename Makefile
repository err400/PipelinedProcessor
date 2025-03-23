# Makefile for Pipelined Processor

CXX = g++
CXXFLAGS = -Wall -std=c++11 -g

SRC_DIR = src
HEADERS_DIR = $(SRC_DIR)/Headers
SRCS = $(SRC_DIR)/Basic_Processor.cpp \
       $(SRC_DIR)/InstructionFile.cpp \
       $(SRC_DIR)/Memory.cpp \
       $(SRC_DIR)/Processor.cpp \
       $(SRC_DIR)/Registers.cpp \
       $(SRC_DIR)/Utils.cpp \
       $(SRC_DIR)/main.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = pipelined_processor

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(HEADERS_DIR) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run