CXX = g++

PROJECTNAME = BabbageVM
OUTPUT_DIR = build

INCLUDE_DIRS = -Iinclude/SDL2 -Iinclude/imgui
LIB_DIRS = -Llib

LIBS = -lmingw32 -lSDL2main -lSDL2

#SRCS = $(wildcard *.cpp) $(wildcard include/imgui/*.cpp)

SRC = ui.cpp $(wildcard imgui/*.cpp)

CXXFLAGS = -Wall -Wextra -std=c++17

default:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUTPUT_DIR)/$(PROJECTNAME) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS)