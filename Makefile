clean:
	rm main

all:
	g++ main.cpp cpu.cpp memory.cpp -o main

run: all
	./main