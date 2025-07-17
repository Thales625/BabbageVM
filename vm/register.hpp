#pragma once

#include "vm_types.hpp"

#include <bitset>

template <typename T=word_t>
struct Register {
public:
	inline T read() const {
		return this->value;
	}
	
	inline void write(T n_value) {
		this->value = n_value;
	}

	std::string to_string() const {
		std::bitset<sizeof(T)*8> bits(this->value);
		return bits.to_string();
	}
private:
	T value;
};