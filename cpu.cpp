#include "cpu.hpp"

CPU::CPU(size_t mem_size) : memory(Memory(mem_size)) {};

void CPU::reset() {
	this->acc.write(0);

	this->pc.write(0);
	this->sp.write(0);

	this->mop.write(0);

	this->ri.write(0);
	this->re.write(0);
	this->r0.write(0);
	this->r1.write(0);

	// clear memory
	this->memory.clear();
}


word_t CPU::ALU::update_flags(word_t value) {
	this->N = value < 0 ? 1 : 0;
	this->Z = value == 0 ? 1 : 0;

	// TODO
	// this->C = value ? 1 : 0;
	// this->V = value ? 1 : 0;

	return value;
}

// op
word_t CPU::ALU::add(word_t a, word_t b) {
	return this->update_flags(a + b);
}

word_t CPU::ALU::sub(word_t a, word_t b) {
	return this->update_flags(a - b);
}

word_t CPU::ALU::mul(word_t a, word_t b) {
	return this->update_flags(a * b);
}

void CPU::print_state() {
	printf("CPU STATE\n\t");
	printf ("acc = 0x%04x\n\t", this->acc);

	printf ("pc = 0x%04x\n\t", this->pc);
	printf ("sp = 0x%04x\n\t", this->sp);
	
	printf ("mop = %04x\n\t", this->mop);

	printf ("ri = 0x%04x\n\t", this->ri);
	printf ("re = 0x%04x\n\t", this->re);
	printf ("r0 = 0x%04x\n\t", this->r0);
	printf ("r1 = 0x%04x\n", this->r1);
}