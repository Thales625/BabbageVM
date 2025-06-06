#pragma once

#include "vm_types.hpp"
#include "register.hpp"
#include "memory.hpp"

class CPU {
public:
	CPU(size_t mem_size);

	Memory memory; // memory addrs?

	void reset(); // set registers, flags, memory to zero

	void cycle();

	void print_state();
private:
	Register<word_t> acc;

	Register<word_t> pc;
	Register<word_t> sp;

	Register<byte_t> mop;
	Register<word_t> ri, re, r0, r1;

	struct ALU {
		// flags
		bit_t N, Z, C, V;

		// op
		word_t add(word_t a, word_t b);
		word_t sub(word_t a, word_t b);
		word_t mul(word_t a, word_t b);
	private:
		word_t update_flags(word_t value);
	};

	// TODO
	word_t fetch() const;
	void decode();
	void execute();
};