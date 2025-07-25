#include <string>
#include <bitset>
#include <iostream>

#include "vm_types.hpp"

#include "cpu.hpp"
#include "bitmap.hpp"
#include "memory.hpp"

int main() {
	Memory<word_t> *mem_ptr = new Memory<word_t>(1024); // 2kB

	CPU cpu(mem_ptr);

	cpu.reset();
 
	std::vector<word_t> program = {
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0b00 // END: STOP
	};

	// std::vector<word_t> data = {
	// 	1, // mem[100]
	// 	1, // mem[101]
	// };
	
	mem_ptr->load(program, 0);
	// mem_ptr->load(data, 100);

	/*
	for (size_t i=0; i<15; i++) {
		printf("mem[%ld] = 0x%04x \n", i, mem_ptr->read(i));
	}
	std::cout << "SIZE: " << mem_ptr->get_size() << "\n";
	*/

	// bitmap display
	// BitMap bitmap{mem_ptr, 100, 5, 5};

	// cpu.run(bitmap);
	cpu.run();

	cpu.print_state();

	delete mem_ptr;
	return 0;
}