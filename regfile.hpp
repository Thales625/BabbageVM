#pragma once

#include "vm_types.hpp"
#include "register.hpp"

#include <vector>
#include <stdexcept>

enum REGS {
	ACC,
	PC,
	SP,
	R0,
	R1
};

template <typename T=word_t>
class RegisterFile {
public:
	RegisterFile(size_t size) : registers(size) {};

	inline size_t get_size() const {
		return this->registers.size();
	}

	T read(size_t index) const {
		// if (index >= this->get_size()) {
		// 	throw std::out_of_range("RegFile read out of bounds");
		// }
		return registers[index].read();
	};

	void write(size_t index, T value) {
		if (index >= this->get_size()) {
			throw std::out_of_range("RegFile write out of bounds");
		}
		registers[index].write(value);
	};

	Register<T>* get_register(size_t index) {
		if (index >= this->get_size()) {
			throw std::out_of_range("RegFile get reg out of bounds");
		}
		return &registers[index];
	};
private:
	std::vector<Register<T>> registers;
	size_t size;
};