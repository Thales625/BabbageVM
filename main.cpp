#include <string>
#include <bitset>
#include <iostream>

#include "cpu.hpp"
#include "memory.hpp"
#include "vm_types.hpp"
#include "utils.hpp"

#include "macro.hpp"
#include "assembler.hpp"
#include "linker.hpp"

// #define DEBUG
// #define BITMAP

#ifdef BITMAP
#include "bitmap.hpp"
#endif

int main(int argc, char* argv[]) {
    // std::string filename = argv[1];

	// std::vector<std::string> lines = Macro::ProcessFromFile(filename);
	// save_file(lines, "bin/macro_out.bbg");

	Assembler assembler;
	assembler.assemble("bin/macro_out.bbg");
	return 0;

	Linker linker;
	linker.link({"bin/assembler_out.OBJ"}, "bin/linker_out.hpx");

	// for (auto& line : lines) {
	// 	std::cout << line << "\n";
	// }

	return 0;

	Memory<word_t> *mem_ptr = new Memory<word_t>(1024); // 2kB

	CPU cpu(mem_ptr);

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