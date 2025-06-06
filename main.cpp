#include <string>
#include <bitset>
#include <iostream>

#include "vm_types.hpp"

#include "cpu.hpp"

int main() {
	CPU cpu(1024);

	cpu.reset();
 
	std::vector<word_t> program = {
		0x10FF,
		0x200A,
		0x300B,
	};

	std::vector<word_t> data = {
		1,
		2,
		3
	};
	cpu.memory.load(program, 0);
	cpu.memory.load(data, 3);

	for (size_t i=0; i<6; i++) {
		// std::cout << "I: " << i << int_to_hex<word_t>(cpu.memory.read(i)) << "\n";
		printf("mem[%d] = 0x%04x \n", i, cpu.memory.read(i));
	}

	std::cout << "SIZE: " << cpu.memory.get_size() << "\n";
	
	cpu.acc.write(10);

	cpu.print_state();

	return 0;
}