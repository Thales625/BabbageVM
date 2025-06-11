#include "cpu.hpp"

#include <iostream>

#define DEBUG

// ctor
CPU::CPU(size_t mem_size) : memory(Memory(mem_size)) {};

// methods
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

void CPU::print_state() {
	std::cout << "CPU STATE\n";
	std::cout << "\tacc = 0b" << this->acc.to_string() << " (" << this->acc.read() << ")\n";

	std::cout << "\tpc = 0b" << this->pc.to_string() << "\n";
	std::cout << "\tsp = 0b" << this->sp.to_string() << "\n";
	
	std::cout << "\tmop = 0b" << this->mop.to_string() << "\n";

	std::cout << "\tri = 0b" << this->ri.to_string() << "\n";
	std::cout << "\tre = 0b" << this->re.to_string() << "\n";
	std::cout << "\tr0 = 0b" << this->r0.to_string() << "\n";
	std::cout << "\tr1 = 0b" << this->r1.to_string() << "\n";
}

void CPU::run() {
	this->running = true;

	while (this->running) {
		this->cycle();
	}
}

void CPU::cycle() {
	// FETCH
	word_t raw_instr = this->memory.read(this->pc.read());
	this->ri.write(raw_instr);
	this->pc.write(this->pc.read() + 1);

	// DECODE
	byte_t base_opcode = get_base_opcode(raw_instr);
	
	#ifdef DEBUG
	std::cout << "OPCODE: " << static_cast<int>(base_opcode) << " | ";
	#endif
	
	// EXECUTE
	// 0 operands
	switch (base_opcode) {
		case 16: i_ret(); return;
		case 11: i_stop(); return;
		case 18: i_pop(); return;
	}
	
	// 1 operand
	AddrMode opd1_addr_mode = decode_mode_opd1(raw_instr);

	#ifdef DEBUG
	std::cout << "OPD1 MODE: " << static_cast<int>(opd1_addr_mode) << "(";
	switch (opd1_addr_mode) {
		case AddrMode::immediate: std::cout << "IMMEDIATE"; break;
		case AddrMode::indirect: std::cout << "INDIRECT"; break;
		case AddrMode::direct: std::cout << "DIRECT"; break;
	}
	std::cout << ")\n";
	#endif

	// FETCH opd_1
	word_t opd_1 = this->memory.read(this->pc.read());
	this->pc.write(this->pc.read() + 1);

	switch (opd1_addr_mode) {
		case AddrMode::immediate: break;
		case AddrMode::indirect:  opd_1 = this->memory.read(this->memory.read(opd_1)); break;
		case AddrMode::direct:    opd_1 = this->memory.read(opd_1); break;
	}

	switch (base_opcode) {
		case 02: i_add(opd_1); return;
		case 00: i_br(opd_1); return;
		case 05: i_brneg(opd_1); return;
		case 01: i_brpos(opd_1); return;
		case 04: i_brzero(opd_1); return;
		case 15: i_call(opd_1); return;
		case 10: i_divide(opd_1); return;
		case 03: i_load(opd_1); return;
		case 14: i_mult(opd_1); return;
		case 17: i_push(opd_1); return;
		case 07: i_store(opd_1); return;
		case 06: i_sub(opd_1); return;
	}

	// 2 operands
	AddrMode opd2_addr_mode = decode_mode_opd1(raw_instr);

	#ifdef DEBUG
	std::cout << "OPD2 MODE: " << static_cast<int>(opd2_addr_mode) << "(";
	switch (opd2_addr_mode) {
		case AddrMode::immediate: std::cout << "IMMEDIATE"; break;
		case AddrMode::indirect: std::cout << "INDIRECT"; break;
		case AddrMode::direct: std::cout << "DIRECT"; break;
	}
	std::cout << ")\n";
	#endif

	// FETCH opd_2
	word_t opd_2 = this->memory.read(this->pc.read());
	this->pc.write(this->pc.read() + 1);

	switch (opd2_addr_mode) {
		case AddrMode::immediate: break;
		case AddrMode::indirect:  opd_2 = this->memory.read(this->memory.read(opd_2)); break;
		case AddrMode::direct:    opd_2 = this->memory.read(opd_2); break;
	}

	switch (base_opcode) {
		case 13: i_copy(opd_1, opd_2); break;
		default:
			std::cerr << "Unknown opcode: " << static_cast<int>(base_opcode) << "\n";
			break;
	}
}

// instructions
void CPU::i_add(word_t opd1) {
	#ifdef DEBUG
	std::cout << "ADD => opd1: " << opd1 << "\n";
	#endif
	this->acc.write(this->alu.add(this->acc.read(), opd1));
}

void CPU::i_br(word_t opd1) {
	#ifdef DEBUG
	std::cout << "BR => opd1: " << opd1 << "\n";
	#endif
	this->pc.write(opd1);
}

void CPU::i_brneg(word_t opd1) {
	#ifdef DEBUG
	std::cout << "BRNEG => opd1: " << opd1 << "\n";
	#endif
	if (this->acc.read()<0) this->pc.write(opd1);
}

void CPU::i_brpos(word_t opd1) {
	#ifdef DEBUG
	std::cout << "BRPOS => opd1: " << opd1 << "\n";
	#endif
	if (this->acc.read()>0) this->pc.write(opd1);
}

void CPU::i_brzero(word_t opd1) {
	#ifdef DEBUG
	std::cout << "BRZERO => opd1: " << opd1 << "\n";
	#endif
	if (this->acc.read()==0) this->pc.write(opd1);
}

void CPU::i_call(word_t opd1) {
	#ifdef DEBUG
	std::cout << "CALL => opd1: " << opd1 << "\n";
	#endif
	this->sp.write(this->pc.read());
	this->pc.write(opd1);
}

void CPU::i_copy(word_t opd1, word_t opd2) {
	#ifdef DEBUG
	std::cout << "COPY => opd1: " << opd1 << "\n";
	#endif
	this->memory.write(opd1, opd2);
}

void CPU::i_divide(word_t opd1) {
	#ifdef DEBUG
	std::cout << "DIVIDE => opd1: " << opd1 << "\n";
	#endif
	this->acc.write(this->alu.div(this->acc.read(), opd1));
}

void CPU::i_load(word_t opd1) {
	#ifdef DEBUG
	std::cout << "LOAD => opd1: " << opd1 << "\n";
	#endif
	this->acc.write(opd1);
}

void CPU::i_mult(word_t opd1) {
	#ifdef DEBUG
	std::cout << "MULT => opd1: " << opd1 << "\n";
	#endif
	this->acc.write(this->alu.mul(this->acc.read(), opd1));
}

void CPU::i_push(word_t opd1) {
	#ifdef DEBUG
	std::cout << "PUSH => opd1: " << opd1 << "\n";
	#endif
	this->memory.write(this->sp.read(), opd1);
	this->sp.write(this->sp.read() + 1);
}

void CPU::i_pop() {
	#ifdef DEBUG
	std::cout << "POP" << "\n";
	#endif
	this->acc.write(this->memory.read(this->sp.read()));
	this->sp.write(this->sp.read() - 1);
}

void CPU::i_ret() {
	#ifdef DEBUG
	std::cout << "RET\n";
	#endif
	this->pc.write(this->memory.read(this->sp.read()));
	this->sp.write(this->sp.read() - 1);
}

void CPU::i_stop() {
	#ifdef DEBUG
	std::cout << "STOP\n";
	#endif
	this->running = false;
}

void CPU::i_store(word_t opd1) {
	#ifdef DEBUG
	std::cout << "STORE => opd1: " << opd1 << "\n";
	#endif
	this->memory.write(opd1, this->acc.read());
}

void CPU::i_sub(word_t opd1) {
	#ifdef DEBUG
	std::cout << "SUB => opd1: " << opd1 << "\n";
	#endif
	this->acc.write(this->alu.add(this->acc.read(), opd1));
}