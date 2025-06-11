#pragma once

#include "vm_types.hpp"

struct ALU {
	// flags
	bit_t N, Z, C, V;

	// op
	word_t add(word_t a, word_t b);
	word_t sub(word_t a, word_t b);
	word_t mul(word_t a, word_t b);
	word_t div(word_t a, word_t b);
private:
	word_t update_flags(word_t value);
};