#include <string>
#include <bitset>
#include <iostream>

#include "vm_types.hpp"

#include "cpu.hpp"
#include "bitmap.hpp"

int main() {
	CPU cpu(1024);

	cpu.reset();
 
	std::vector<word_t> program = {
		0x0302, 5,  // LOAD 5
		0x0601, 21, // SUB  mem[mem[21]]
		0x0b00      // STOP
	};

	std::vector<word_t> data = {
		1,  // mem[20] = 1
		20, // mem[21] = 20
	};
	
	cpu.memory.load(program, 0);
	cpu.memory.load(data, 20);
	
	/*
	for (size_t i=0; i<15; i++) {
		printf("mem[%ld] = 0x%04x \n", i, cpu.memory.read(i));
	}
	std::cout << "SIZE: " << cpu.memory.get_size() << "\n";
	*/

	// bitmap display
	BitMap bitmap{&cpu.memory, 100, 5, 5};

	cpu.run(bitmap);

	cpu.print_state();

	return 0;
}