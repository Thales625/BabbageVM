#pragma once

#include <cstddef>
#include "vm_types.hpp"

struct MemAllocation {
public:
	MemAllocation(size_t n_start_index, size_t n_end_index) : start_index(n_start_index), end_index(n_end_index) {}
private:
	size_t start_index, end_index;
};

enum class AddrMode { immediate, reg_direct, reg_indirect, mem_direct, mem_indirect };

inline AddrMode decode_mode_opd(word_t instr) {
	instr &= 0b111;

	if (instr == 0b10) return AddrMode::immediate;

	if (instr & 0b100 == 0b100) { // MEM
		instr &= 0b011;
		if (instr == 0b00) return AddrMode::mem_direct;
		if (instr == 0b01) return AddrMode::mem_indirect;
	} else { // REG
		instr &= 0b011;
		if (instr == 0b00) return AddrMode::reg_direct;
		if (instr == 0b01) return AddrMode::reg_indirect;
	}
	return AddrMode::immediate;
}

inline byte_t get_base_opcode(word_t instr) {
	return static_cast<byte_t>(instr >> 8);
}