#include <string>
#include <bitset>
#include <iostream>

#include "vm_types.hpp"

#include "cpu.hpp"
#include "bitmap.hpp"
#include "memory.hpp"

// #define DEBUG
// #define BITMAP

#ifdef BITMAP
#include "bitmap.hpp"
#endif

int main() {
	Memory<word_t> *mem_ptr = new Memory<word_t>(1024); // 2kB

	CPU cpu(mem_ptr);

	cpu.reset();

	/*
	03 10      ; LOAD  @0x10   (carrega A no acumulador)
    02 11      ; ADD   @0x11   (soma B)
    07 12      ; STORE @0x12   (salva resultado em C)
    11         ; STOP

	Dados
	10:     05         ; A = 5
	11:     03         ; B = 3
	12:     00         ; C = 0 (local para armazenar resultado)
	*/
 
	std::vector<word_t> program = {
		0x0000, // LOAD  @0x10 (carrega A no acumulador)
		0x0000, // ADD   @0x11 (soma B)
		0x0000, // STORE @0x12 (salva resultado em C)
		0x0b00  // STOP
	};

	std::vector<word_t> data = {
		5, // mem[10]
		3, // mem[11]
		0, // mem[12]
	};
	
	mem_ptr->load(program, 0);
	mem_ptr->load(data, 10);

	/*
	for (size_t i=0; i<15; i++) {
		printf("mem[%ld] = 0x%04x \n", i, mem_ptr->read(i));
	}
	std::cout << "SIZE: " << mem_ptr->get_size() << "\n";
	*/

	#ifdef BITMAP
	BitMap bitmap{mem_ptr, 100, 5, 5};
	cpu.run(bitmap);
	#else
	cpu.run();
	#endif

	cpu.print_state();

	delete mem_ptr;
	return 0;
}