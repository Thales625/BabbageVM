#pragma once

#include "vm_types.hpp"
#include "register.hpp"
#include "memory.hpp"
#include "utils.hpp"
#include "alu.hpp"

class CPU {
public:
	CPU(size_t mem_size);

	Memory memory;

	void reset(); // set registers, flags, memory to zero

	void run();

	void cycle();

	void print_state();
private:
	bool running;

	Register<word_t> acc;

	Register<word_t> pc;
	Register<word_t> sp;

	Register<byte_t> mop;
	Register<word_t> ri, re, r0, r1;

	ALU alu;

	// instructions

	// 0 op
	void i_ret();
	void i_stop();
	void i_pop();

	// 1 op
	void i_add(word_t opd1);
	void i_br(word_t opd1);
	void i_brneg(word_t opd1);
	void i_brpos(word_t opd1);
	void i_brzero(word_t opd1);
	void i_call(word_t opd1);
	void i_divide(word_t opd1);
	void i_load(word_t opd1);
	void i_mult(word_t opd1);
	void i_push(word_t opd1);
	void i_store(word_t opd1);
	void i_sub(word_t opd1);

	// 2 op
	void i_copy(word_t opd1, word_t opd2);
};