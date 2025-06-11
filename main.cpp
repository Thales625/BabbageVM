#include <string>
#include <bitset>
#include <iostream>

#include "vm_types.hpp"

#include "cpu.hpp"

int main() {
	CPU cpu(1024);

	cpu.reset();
 
	std::vector<word_t> program = {
		0x0300, 10, // LOAD mem[10]
		0x0200, 11, // ADD  mem[11]
		0x0202, 3,  // ADD  value 3
		0x0b00      // STOP
	};

	std::vector<word_t> data = {
		1, // mem[10] = 1
		33 // mem[11] = 2
	};
	
	cpu.memory.load(program, 0);
	cpu.memory.load(data, 10);

	for (size_t i=0; i<15; i++) {
		printf("mem[%ld] = 0x%04x \n", i, cpu.memory.read(i));
	}

	std::cout << "SIZE: " << cpu.memory.get_size() << "\n";

	cpu.run();

	cpu.print_state();

	return 0;
}