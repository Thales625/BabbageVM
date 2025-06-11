#include "alu.hpp"

word_t ALU::update_flags(word_t value) {
	this->N = value < 0 ? 1 : 0;
	this->Z = value == 0 ? 1 : 0;

	// TODO
	// this->C = value ? 1 : 0;
	// this->V = value ? 1 : 0;

	return value;
}

// op
word_t ALU::add(word_t a, word_t b) {
	return this->update_flags(a + b);
}

word_t ALU::sub(word_t a, word_t b) {
	return this->update_flags(a - b);
}

word_t ALU::mul(word_t a, word_t b) {
	return this->update_flags(a * b);
}

word_t ALU::div(word_t a, word_t b) {
	return this->update_flags(a / b);
}