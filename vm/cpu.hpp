#pragma once

#include "vm_types.hpp"
#include "register.hpp"
#include "regfile.hpp"
#include "memory.hpp"
#include "vm_utils.hpp"
#include "bitmap.hpp"

class CPU {
public:
	CPU(Memory<word_t> *n_mem_ptr);

	Memory<word_t> *mem_ptr;

	void reset(); // set registers, memory to zero

	void set_entry(word_t line);

	void run(BitMap &bitmap);
	void run();

	void cycle();

	void print_state() const;
private:
	bool running;

	RegisterFile<word_t> reg_file;

	Register<word_t> *acc, *pc, *sp, *r0, *r1;

	Register<word_t> ri, re;
	Register<byte_t> mop;

	// instructions

	// 0 op
	void i_ret();
	void i_stop();

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
	void i_pop(word_t opd1);

	// 2 op
	void i_copy_r(word_t opd1, word_t opd2);
	void i_copy_m(word_t opd1, word_t opd2);
};