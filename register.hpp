#pragma once

#include "vm_types.hpp"

template <typename T=word_t>
struct Register {
public:
	T read() const {
		return this->value;
	}
	
	void write(T n_value) {
		this->value = n_value;
	}
private:
	T value;
};