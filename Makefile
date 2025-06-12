CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

TARGET = main

SRCS = main.cpp cpu.cpp memory.cpp bitmap.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./main

clean:
	rm -f *.o $(TARGET)