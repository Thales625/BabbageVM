#pragma once

#include <cstddef>
#include "vm_types.hpp"

struct MemAllocation {
public:
	MemAllocation(size_t n_start_index, size_t n_end_index) : start_index(n_start_index), end_index(n_end_index) {}
private:
	size_t start_index, end_index;
};

enum class AddrMode { direct, indirect, immediate };

inline AddrMode decode_mode_opd1(word_t instr) {
	instr &= 0b11;
	if (instr == 0b00) return AddrMode::direct;
	if (instr == 0b01) return AddrMode::indirect;
	// if (instr == 0b10) return AddrMode::immediate;
	return AddrMode::immediate;
}

inline AddrMode decode_mode_opd2(word_t instr) {
	instr &= 0b11;
	if (instr == 0b00) return AddrMode::direct;
	if (instr == 0b01) return AddrMode::indirect;
	// if (instr == 0b10) return AddrMode::immediate;
	return AddrMode::immediate;
}

inline byte_t get_base_opcode(word_t instr) {
	return static_cast<byte_t>(instr >> 8);
}