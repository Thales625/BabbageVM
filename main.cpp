#include <string>
#include <bitset>
#include <iostream>

#include "vm_types.hpp"

#include "cpu.hpp"
#include "bitmap.hpp"

int main() {
	Memory *mem_ptr = new Memory(1024); // 2kB

	CPU cpu(mem_ptr);

	cpu.reset();

	#define END 37
	#define LOOP 13
 
	std::vector<word_t> program = {
		0x0d22, 6, 0, // $FIB0 = 0
		0x0d22, 7, 1, // $FIB1 = 1

		0x0302, 4, // $ACC = N
		0x0602, 2, // $ACC -= 2
		0x0d02, 5, REGS::ACC, // $I = $ACC

		0x0300, 7, // LOOP: $ACC = $FIB1
		0x0200, 6, // $ACC += $FIB0
		0x0d02, 8, REGS::ACC, // $TEMP = $ACC

		0x0d02, 6, 7, // $FIB0 = $FIB1
		0x0d02, 7, 8, // $FIB0 = $TEMP

		0x0300, 5, // $ACC = $I
		0x0602, 1, // $ACC -= 1
		0x0402, END, // BR_ZERO(END)
		0x0d02, 5, REGS::ACC, // $I = $ACC
		0x0002, LOOP, // BR(LOOP)

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