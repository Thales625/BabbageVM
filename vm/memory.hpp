#pragma once

#include "vm_types.hpp"
#include "register.hpp"

#include <vector>
#include <stdexcept>

template <typename T=word_t>
class Memory {
public:
	Memory(std::size_t size) : data(size, 0) {};

	size_t get_addressable_size() const {
		return this->data.size();
	};

	size_t get_size() const {
		return this->data.size() * sizeof(T);
	};

	T read(address_t address) const {
		if (address >= this->get_size()) {
			throw std::out_of_range("Memory read out of bounds");
		}
		return data[address];
	};

	void write(address_t address, T value) {
		if (address >= this->get_size()) {
			throw std::out_of_range("Memory write out of bounds");
		}
		data[address] = value;
	};

	void clear() {
		std::fill(data.begin(), data.end(), 0);
	};

	void load(const std::vector<T>& data_vector, address_t start_address) {
		if (start_address + data_vector.size() > this->get_size()) {
			throw std::out_of_range("Data does not fit in memory at the specified address");
		}

		std::copy(data_vector.begin(), data_vector.end(), this->data.begin() + start_address);
	};
private:
	std::vector<T> data;
};